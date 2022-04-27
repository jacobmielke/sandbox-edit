#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
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
#include "placeable.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, Heightmap);
void draw_gui(Camera camera, Heightmap height_map, int, Placeable obj);

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
    Shader model_shader("shaders/v_basic.glsl", "shaders/f_basic.glsl");
    Shader tool_shader("shaders/v_tool.glsl", "shaders/f_tool.glsl");
    Shader heightmap_shader("shaders/heightmap_vertex.glsl", "shaders/heightmap_fragment.glsl");
    Shader instance_shader("shaders/v_instance.glsl", "shaders/f_instance.glsl");

    // Load models
    Model pine_tree("models/tree_pine/Tree.obj");
    Model shovel("models/shovel/shovel.obj");
    Model bucket("models/bucket/bucket.obj");
    Model grass_model("models/grass/grass.obj");
    Model rock_model("models/rock/Rock.obj");
    Model rabbit_model("models/rabbit/rabbit.obj");
    Model water_model("models/water/water0.obj");
    Model dock_model("models/dock/dock.obj");
    Model boat_model("models/boat/v_boat.obj");
    Model cactus_model("models/cactus/cactus.obj");
    Model house("models/house/p3d_medieval_enterable_bld-13.obj");

    // Load heightmap
    Heightmap height_map("heightmaps/512x512.png");
    camera.Position = glm::vec3(height_map.height / 2, 3.0f, height_map.width/2); // Set default position

    // Create instances list
    vector<Instance> instance_draw_stack; // This will be used in render loop

    // Create Placeable
    vector<Model> temp = { house,water_model,rabbit_model,dock_model,boat_model,cactus_model};
    Placeable place_objects(temp);

    // Create Instances to draw
    Instance grass(1, grass_model); // Create grass
    Instance rock(1, rock_model);
    Instance pinetree(1, pine_tree); // Create Tree


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
        if(CURRENT_TOOL == -1 || CURRENT_TOOL == 1 || CURRENT_TOOL == 0 || CURRENT_TOOL == 3)
            camera.ray_to_world();

        if (POP_STACK)
        {
            POP_STACK = false;
            if (!instance_draw_stack.empty())
            {
                instance_draw_stack.pop_back();
            }
        }
        // Check for
        if (WORLD_PLACE_OBJECTS)
        {
            WORLD_PLACE_OBJECTS = false;

            switch (SELECTED_ITEM_INDEX_FOLIAGE)
            {
            case 0: // Grass
                grass.inst_numb = PLACE_NUMBER;
                grass.create_positions(height_map, height_map.width, height_map.height, deltaTime, WORLD_SPAWN_MIN, WORLD_SPAWN_MAX);
                grass.setup_buffer();
                instance_draw_stack.push_back(grass);
                break;
            case 1: // Tree
                pinetree.inst_numb = PLACE_NUMBER;
                pinetree.create_positions(height_map, height_map.width, height_map.height, deltaTime, WORLD_SPAWN_MIN, WORLD_SPAWN_MAX);
                pinetree.setup_buffer();
                break;
            case 2: // Rocks
                rock.inst_numb = PLACE_NUMBER;
                rock.create_positions(height_map, height_map.width, height_map.height, deltaTime, WORLD_SPAWN_MIN, WORLD_SPAWN_MAX);
                rock.setup_buffer();
                instance_draw_stack.push_back(rock);
                break;
            }

            
        }

        // Render instances
        if (!instance_draw_stack.empty())
        {
            for (unsigned int j = 0; j < instance_draw_stack.size(); j++)
            {
                instance_shader.use();
                instance_shader.setMat4("projection", projection);
                instance_shader.setMat4("view", view);
                instance_draw_stack[j].draw(instance_shader);
            }
        }

        // Draw Instanced Trees 
        model_shader.use();
        model_shader.setMat4("projection", projection);
        model_shader.setMat4("view", view);
        if (pinetree.established)
        {
            for (unsigned int i = 0; i < pinetree.inst_numb; i++)
            {
                model_shader.setMat4("model", pinetree.model_matrix[i]);
                pinetree.model[0].Draw(model_shader);
            }
        }

        // Draw Placed Items
        if (PLACEABLE_POP)
        {
            PLACEABLE_POP = false;
            place_objects.model_matrix.pop_back();
            place_objects.index_stack.pop_back();
        }

        model_shader.use();
        model_shader.setMat4("projection", projection);
        model_shader.setMat4("view", view);
        place_objects.draw(model_shader);

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
        if (CURRENT_TOOL == 1 || CURRENT_TOOL == 3)
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

        if (CURRENT_TOOL == 0)
        {
            model_shader.use();
            model_shader.setMat4("projection", projection);
            model_shader.setMat4("view", view);
            place_objects.draw_cursor(model_shader, height_map, camera.w_xpos, camera.w_ypos, camera.w_zpos);

            if (PLACE_OBJECT)
            {
                PLACE_OBJECT = false;
                place_objects.place();
            }
        }

        // Heightmap
        if (HEIGHTMAP_RELOAD)
        {
            HEIGHTMAP_RELOAD = false;
            height_map.reload_heightmap(height_map.filenames[HEIGHTMAP_INDEX]);
            camera.Position = glm::vec3(height_map.height / 2, 3.0f, height_map.width / 2); 
        }
        if (HEIGHTMAP_EXPORT)
        {
            HEIGHTMAP_EXPORT = false;
            height_map.export_heightmap(height_map.width, height_map.height, height_map.nChannels);
        }
        
        heightmap_shader.use();
        height_map.activate_textures(heightmap_shader);
        glm::mat4 map = glm::mat4(1.0f);
        //map = glm::translate(map, glm::vec3(-height_map.height / 2, 0.0f, -height_map.width / 2));
        heightmap_shader.setMat4("model", map);
        heightmap_shader.setMat4("projection", projection);
        heightmap_shader.setMat4("view", view);

        // Set mouse
        heightmap_shader.setFloat("delta_time", deltaTime);
        heightmap_shader.setFloat("mouse_x", camera.w_xpos);
        heightmap_shader.setFloat("mouse_z", camera.w_zpos);
        height_map.draw(heightmap_shader);
        
        // Draw Gui
        draw_gui(camera, height_map, instance_draw_stack.size(), place_objects);

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
void draw_gui(Camera camera, Heightmap height_map, int stack_size, Placeable obj)
{
    static const char* inst_list[]{"Grass", "Tree", "Rock"};
    ImGui::Begin("Option Menu");
    if (ImGui::CollapsingHeader("Heightmap Settings"))
    {
        float x = 2.0f * (camera.xpos / SCR_WIDTH) - 1.0f;
        float y = 2.0f * -(camera.ypos / SCR_HEIGHT) + 1.0f; // Normalize

        ImGui::Text("Dimensions: %d x %d", height_map.width, height_map.height);
        ImGui::Text("Mouse Screen Coords: %f x %f", x, y);
        ImGui::Text("Mouse World Coords: %f x %f x %f", camera.w_xpos, camera.w_ypos, camera.w_zpos);
        ImGui::Text("Camera Coords: %f x %f x %f", camera.Position.x, camera.Position.y, camera.Position.z);
        ImGui::Checkbox("Wireframe mode", &WIREFRAME);
        ImGui::ListBox("Filename", &HEIGHTMAP_INDEX, height_map.filenames, 5);
        if (ImGui::Button("Load Heightmap"))
        {
            HEIGHTMAP_RELOAD = true;
        }

        if (ImGui::Button("Export Current Heightmap"))
        {

            HEIGHTMAP_EXPORT = true;
        }
        
        //ImGui::InputText("Custom File Path", CUSTOM_FILEPATH,sizeof CUSTOM_FILEPATH);
        //if (ImGui::Button("Load Custom Heightmap"))
        //{

        //    height_map.recreate_custom(CUSTOM_FILEPATH);
        //}
       
    }   
    if (ImGui::CollapsingHeader("Terrain Tools"))
    {
        ImGui::Text("Current Tool: %d", CURRENT_TOOL);
        ImGui::RadioButton("Raise Terrain", &CURRENT_TOOL, 1); ImGui::SameLine();
        ImGui::RadioButton("Lower Terrain", &CURRENT_TOOL, -1); ImGui::SameLine();
        ImGui::RadioButton("Average Terrain", &CURRENT_TOOL, 3); ImGui::SameLine();
        ImGui::RadioButton("View Mode", &CURRENT_TOOL, 2);
        ImGui::SliderFloat("Radius", &TOOL_RADIUS, TOOL_RADIUS_MIN, TOOL_RADIUS_MAX); 
     

        ImGui::SliderFloat("Intensity", &TOOL_INTENSITY, TOOL_INTENSITY_MIN, TOOL_INTENSITY_MAX); 
        ImGui::InputFloat("", &TOOL_INTENSITY);

        //ImGui::SliderFloat("Opacity", &TOOL_OPACITY, TOOL_OPACITY_MIN, TOOL_OPACITY_MAX); 
        
    }

    if (ImGui::CollapsingHeader("Place Objects"))
    {
        ImGui::RadioButton("Place Mode", &CURRENT_TOOL, 0);
        if (CURRENT_TOOL == 0) // If place mode
        {
            static const char* obj_list[]{ "House", "Water", "Rabbit", "Dock", "Boat", "Cactus"};
            ImGui::SliderInt("Object Rotation", &PLACEABLBE_ROTATION, 0, 360); ImGui::SameLine();
            ImGui::RadioButton("View Mode", &CURRENT_TOOL, 2);
            ImGui::SliderFloat("Object Scale", &PLACEABLE_SCALE, 0.001f, 100.0f, "%.5f"); ImGui::SameLine();
            ImGui::InputFloat("", &PLACEABLE_SCALE);
            ImGui::Checkbox("Clamp Y", &PLACEABLE_YSTRAIN);
            ImGui::Checkbox("X", &PLACEABLE_ROT_X); ImGui::SameLine();
            ImGui::Checkbox("Y", &PLACEABLE_ROT_Y); ImGui::SameLine();
            ImGui::Checkbox("Z", &PLACEABLE_ROT_Z); 

            ImGui::ListBox("Objects", &PLACEABLE_MENU_INDEX, obj_list, 6);
            ImGui::Text("Current Object : %s", obj_list[PLACEABLE_MENU_INDEX]);
            if (ImGui::Button("Place Object"))
            {
                PLACE_OBJECT = true;
            }
            if (ImGui::Button("Pop Object"))
            {
                PLACEABLE_POP = true;
            }

        }
    }
    
    if (ImGui::CollapsingHeader("Foliage  Spawn"))
    {
        ImGui::Text("Adjust spawn height ranges");
        ImGui::SliderFloat("Min Spawn Height", &WORLD_SPAWN_MIN, WORLD_PLACE_MIN, WORLD_PLACE_MAX);
        ImGui::SliderFloat("Max Spawn Height", &WORLD_SPAWN_MAX, WORLD_PLACE_MIN, WORLD_PLACE_MAX);
        ImGui::InputFloat("Place Offset", &WORLD_PLACE_OFFSET);
        ImGui::SliderInt("Spawn Number", &PLACE_NUMBER, PLACE_NUMBER_MIN, PLACE_NUMBER_MAX); ImGui::SameLine();
        ImGui::InputInt("", &PLACE_NUMBER);
        ImGui::ListBox("Foliage Objects", &SELECTED_ITEM_INDEX_FOLIAGE, inst_list, 3);
        ImGui::Text("Current Object : %s", inst_list[SELECTED_ITEM_INDEX_FOLIAGE]);
        if (ImGui::Button("Mass Place Objects"))
        {
            WORLD_PLACE_OBJECTS = true;
        }
        ImGui::Text("Stack size: %d", stack_size);
        if (ImGui::Button("Pop Stack"))
        {
            POP_STACK = true;
        }

        
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
