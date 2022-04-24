#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "shader.h"
#include "filesystem.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "model.h"
#include "heightmap.h"
#include "options.h"
#include "instance.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, Heightmap);
void draw_gui(Camera camera, Heightmap height_map);
glm::mat4* create_mesh_matrix(int inst_numb, int w, int h, float dT, Heightmap map);
float rand_float(float a, float b);

// Global Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool firstMouse = true;

// Delta time
float deltaTime = 0.0f;	
float lastFrame = 0.0f;

int main()
{
    // GLFW init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

       
    // Create window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Sandbox Terrain", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Set GLFW commands
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);


    // GLAD loader
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    //stbi_set_flip_vertically_on_load(true);

    // Load shaders
    Shader model_shader("v_basic.glsl", "f_basic.glsl");
    Shader tool_shader("v_tool.glsl", "f_tool.glsl");
    Shader heightmap_shader("heightmap_vertex.glsl", "heightmap_fragment.glsl");
    Shader instance_shader("v_instance.glsl", "f_instance.glsl");

    // Load models
    Model tree("models/tree_pine/Tree.obj");
    Model shovel("models/shovel/shovel.obj");
    Model bucket("models/bucket/bucket.obj");
    Model grass_model("models/grass/grass.obj");

    // Load heightmap
    Heightmap height_map("heightmaps/512x512.png");

    // Create model list
    glm::mat4* model_list;
    model_list = create_mesh_matrix(WORLD_MAX_TREE, height_map.width, height_map.height, 120.0f, height_map);

    // Create instances list


    Instance grass(WORLD_MAX_GRASS); // Create grass
    grass.create_positions(height_map, height_map.width, height_map.height, 122, WORLD_SPAWN_MIN, WORLD_SPAWN_MAX);
    grass.setup_buffer(grass_model);

    //imGUI
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Render
        glClearColor(0.5294f, 0.8078f, 0.9216f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (WIREFRAME) // Enable wireframe-mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // GUI Setup
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (!io.WantCaptureMouse)
        {
            // Input
            processInput(window, height_map);
            
            if(LEFT_PRESSED)
                height_map.scan_heightmap(camera.w_xpos, camera.w_zpos, camera);
        }

        // Camera and view transformation
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);
        glm::mat4 view = camera.GetViewMatrix();
        if(CURRENT_TOOL == -1 || CURRENT_TOOL == 1)
            camera.ray_to_world();

        // Render Trees
        model_shader.use();
        model_shader.setMat4("projection", projection);
        model_shader.setMat4("view", view);
        for (unsigned int i = 0; i < WORLD_MAX_TREE; i++)
        {
            model_shader.setMat4("model", model_list[i]);
            tree.Draw(model_shader);
        }

        // Render instances
        instance_shader.use();
        instance_shader.setMat4("projection", projection);
        instance_shader.setMat4("view", view);
        grass.draw(instance_shader, grass_model);

        //Shovel Time
        if (CURRENT_TOOL == -1)
        {
            tool_shader.use();
            glm::mat4 shov = glm::mat4(1.0f);
            shov = glm::translate(shov, glm::vec3(camera.w_xpos, camera.w_ypos, camera.w_zpos)); // Center
            shov = glm::rotate(shov, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
            shov = glm::scale(shov, glm::vec3(0.5f * WORLD_SCALE, 0.5f * WORLD_SCALE, 0.5f * WORLD_SCALE));	// Scale
            tool_shader.setMat4("model", shov);
            tool_shader.setMat4("projection", projection);
            tool_shader.setMat4("view", view);
            shovel.Draw(tool_shader);
        }

        // Bucket time
        if (CURRENT_TOOL == 1)
        {
            tool_shader.use();
            glm::mat4 buck = glm::mat4(1.0f);
            buck = glm::translate(buck, glm::vec3(camera.w_xpos, camera.w_ypos, camera.w_zpos)); // Center
            buck = glm::rotate(buck, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            buck = glm::rotate(buck, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
            buck = glm::scale(buck, glm::vec3(0.05f, 0.05f, 0.05f));	// Scale
            tool_shader.setMat4("model", buck);
            tool_shader.setMat4("projection", projection);
            tool_shader.setMat4("view", view);
            bucket.Draw(tool_shader);
        }


        // Render Map
        heightmap_shader.use();
        height_map.activate_textures(heightmap_shader);
        glm::mat4 map = glm::mat4(1.0f);
        map = glm::translate(map, glm::vec3(0.0f, 0.0f, 0.0f));
        map = glm::scale(map, glm::vec3(WORLD_SCALE, WORLD_SCALE, WORLD_SCALE));
        heightmap_shader.setMat4("model", map);
        heightmap_shader.setMat4("projection", projection);
        heightmap_shader.setMat4("view", view);

        // Set mouse
        heightmap_shader.setFloat("delta_time", deltaTime);
        heightmap_shader.setFloat("mouse_x", camera.w_xpos);
        heightmap_shader.setFloat("mouse_z", camera.w_zpos);
        height_map.draw(heightmap_shader);
        
        // Draw Gui
        draw_gui(camera, height_map);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

// Inputs
void processInput(GLFWwindow* window, Heightmap map)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        camera.camera_state = ROAM;
        // GLFW mouse capture
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
    {
        // GLFW mouse capture
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        camera.camera_state = BUILD;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        LEFT_PRESSED = true;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        LEFT_PRESSED = false;
    }


    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 0.0f;
    if (camera.camera_state == ROAM)
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            camera.MovementSpeed = 35.0f;
        }
        else
        {
            camera.MovementSpeed = 13.0f;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Mouse movement
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    camera.xpos = static_cast<float>(xposIn);
    camera.ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        camera.lastX = camera.xpos;
        camera.lastY = camera.ypos;
        firstMouse = false;
    }

    float xoffset = camera.xpos - camera.lastX;
    float yoffset = camera.lastY - camera.ypos;
    camera.lastX = camera.xpos;
    camera.lastY = camera.ypos;

    if (camera.camera_state == ROAM)
    {
        camera.ProcessMouseMovement(xoffset, yoffset, true);
    }

    if (LEFT_PRESSED)
    {
        camera.ProcessMouseMovement(xoffset, yoffset, true);
    }
}

// Scrollwheel
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// GUI
void draw_gui(Camera camera, Heightmap height_map)
{
    ImGui::Begin("Option Menu");
    if (ImGui::CollapsingHeader("World Settings"))
    {
        float x = 2.0f * (camera.xpos / SCR_WIDTH) - 1.0f;
        float y = 2.0f * -(camera.ypos / SCR_HEIGHT) + 1.0f; // Normalize

        ImGui::Text("Dimensions: %d x %d", height_map.width, height_map.height);
        ImGui::Text("Mouse Screen Coords: %f x %f", x, y);
        ImGui::Text("Mouse World Coords: %f x %f x %f", camera.w_xpos, camera.w_ypos, camera.w_zpos);
        ImGui::Text("Camera Coords: %f x %f x %f", camera.Position.x, camera.Position.y, camera.Position.z);
        ImGui::Checkbox("Wireframe mode", &WIREFRAME);
        ImGui::SliderFloat("World Scale", &WORLD_SCALE, 0.01f, 5.0f);
    }
    if (ImGui::CollapsingHeader("Tools"))
    {
        ImGui::RadioButton("Raise Terrain", &CURRENT_TOOL, 1); ImGui::SameLine();
        ImGui::RadioButton("Lower Terrain", &CURRENT_TOOL, -1); ImGui::SameLine();
        ImGui::RadioButton("View Mode", &CURRENT_TOOL, 2);
        ImGui::SliderFloat("Radius", &TOOL_RADIUS, TOOL_RADIUS_MIN, TOOL_RADIUS_MAX);
        ImGui::SliderFloat("Intensity", &TOOL_INTENSITY, TOOL_INTENSITY_MIN, TOOL_INTENSITY_MAX);
        ImGui::SliderFloat("Opacity", &TOOL_OPACITY, TOOL_OPACITY_MIN, TOOL_OPACITY_MAX);
    }

    if (ImGui::CollapsingHeader("Objects"))
    {
        ImGui::RadioButton("Place Mode", &CURRENT_TOOL, 2);
        if (CURRENT_TOOL == 2) // If place mode
        {
            static const char* obj_list[]{ "tree.obj", "grass.obj", "house.obj" };
            
            ImGui::ListBox("Objects", &SELECTED_ITEM_INDEX, obj_list, 3);
            ImGui::Text("Current Object : %s", obj_list[SELECTED_ITEM_INDEX]);
            if (ImGui::Button("Mass Place Objects"))
            {
                WORLD_PLACE_OBJECTS = true;
            }
            ImGui::SliderInt("Intensity", &WORLD_PLACE_INTENSITY, WORLD_PLACE_INTENSITY_MIN, WORLD_PLACE_INTENSITY_MAX);
            ImGui::SliderFloat("Radius", &WORLD_PLACE_RADIUS, WORLD_PLACE_RADIUS_MIN, WORLD_PLACE_RADIUS_MAX);
            ImGui::DragFloatRange2("Min/Max Height", &WORLD_SPAWN_MIN, &WORLD_SPAWN_MAX);
        }
    }
    
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


glm::mat4* create_mesh_matrix(int inst_numb, int w, int h, float dT, Heightmap map)
{
    srand(dT);
    glm::mat4* model_matrix_loc = new glm::mat4[inst_numb];
    glm::vec3* random_point = new glm::vec3[inst_numb]; 
    float scale;
    for (unsigned int i = 0; i < inst_numb; i++) // For every instance generate random x and z points
    {
        int width = w / 2;
        int height = h / 2;
        float x = rand() % (width + 1 - (-width)) + (-width);
        float z = rand() % (height + 1 - (-height)) + (-height);
        float y = map.grab_height(x,z) - 0.2f;
        glm::mat4 curr_model = glm::mat4(1.0f);
        curr_model = glm::translate(curr_model, glm::vec3(x, y, z));
        
        scale = rand_float(0.4f, 0.8f);
        curr_model = glm::scale(curr_model, glm::vec3(scale));

        float rot = (rand() % 360);
        curr_model = glm::rotate(curr_model, rot, glm::vec3(0, 1, 0));

        model_matrix_loc[i] = curr_model;
    }

    return model_matrix_loc;
}


float rand_float(float a, float b)
{
    return ((b - a) * ((float)rand() / RAND_MAX)) + a;
}