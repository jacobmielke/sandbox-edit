#pragma once
#include "model.h"
#include <glm/glm.hpp>
#include "heightmap.h"

class Instance
{
public:
    vector<Model> model;
    int inst_numb,old_size;
    glm::mat4* model_matrix = new glm::mat4[inst_numb]; // Establish model matrix memory
    unsigned int buffer = 0;
    bool established = false;
    int obj_index = 0;
    const char* list_text;
	Instance(int n, Model m)
	{
        this->inst_numb = n;
        model.push_back(m);
	}

    void create_positions(Heightmap hm, int w, int h, float dt, float minh, float maxh)
    {
        established = true;
        model_matrix = create_mesh_matrix(inst_numb, w, h, dt, hm, minh, maxh); // Set the matrix
    }

    void setup_buffer()
    {
        if (established)
        {
            glDeleteBuffers(1, &buffer);
        }

        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, inst_numb * sizeof(glm::mat4), &model_matrix[0], GL_STATIC_DRAW);

        for (unsigned int i = 0; i < model.front().meshes.size(); i++)
        {
            unsigned int VAO = model.front().meshes[i].VAO;
            glBindVertexArray(VAO);
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

            glVertexAttribDivisor(3, 1);
            glVertexAttribDivisor(4, 1);
            glVertexAttribDivisor(5, 1);
            glVertexAttribDivisor(6, 1);

            glBindVertexArray(0);
        }
    }

    void draw(Shader shader)
    {
        shader.use();
        for (int i = 0; i < model[0].meshes[0].textures.size(); i++)
        {
            string number;
            string name = model[0].meshes[0].textures[i].type;
            glActiveTexture(GL_TEXTURE0 + i);

            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            glBindTexture(GL_TEXTURE_2D, model[0].meshes[0].textures[i].id);

        }
        for (unsigned int i = 0; i < model[0].meshes.size(); i++)
        {
            shader.use();
            shader.setInt("texture_diffuse1", 0);
            if(model[0].textures_loaded.size() > 0)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, model[0].textures_loaded[0].id);
            }


            glBindVertexArray(model[0].meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(model[0].meshes[i].indices.size()), GL_UNSIGNED_INT, 0, inst_numb);
            glBindVertexArray(0);
        }

        glActiveTexture(GL_TEXTURE0);
    }

private:
    glm::mat4* create_mesh_matrix(int inst_numb, int w, int h, float dT, Heightmap map, float min_height, float max_height)
    {
        srand(dT);
        glm::mat4* model_matrix_loc = new glm::mat4[inst_numb];
        //glm::vec3* random_point = new glm::vec3[inst_numb];
        float scale,x,y,z;
        for (unsigned int i = 0; i < inst_numb; i++) // For every instance generate random x and z points
        {
            float height = (float)h - 1;
            float width = (float)w - 1;

            x = rand_float(0, height);//rand() % (width + 1 - (-width)) + (-width);
            z = rand_float(0, width);//rand() % (height + 1 - (-height)) + (-height);
            y = map.grab_height(x, z);

            while ((y >= max_height) || (y <= min_height))
            {
                x = rand_float(0, height);//rand() % (width + 1 - (-width)) + (-width);
                z = rand_float(0, width);//rand() % (height + 1 - (-height)) + (-height);
                y = map.grab_height(x, z);
            }

            glm::mat4 curr_model = glm::mat4(1.0f);
            curr_model = glm::translate(curr_model, glm::vec3(x, y + WORLD_PLACE_OFFSET, z));

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


};