#pragma once
#include "model.h"
#include <glm/glm.hpp>
#include "heightmap.h"
#include "camera.h"
#include "options.h"

class Placeable
{
public:
	vector<Model> model_list;
	vector<int> index_stack; // Use to draw models in order
	vector<glm::mat4> model_matrix;
	glm::mat4 steady_model_mat;
	glm::mat4 steady_model_scale;
	glm::mat4 steady_model_rot;
	
	int numb_models, curr_model_selected = 0;
	Placeable(vector<Model> input)
	{
		this->numb_models = input.size();
		this->model_list = input;
		//for (unsigned int i = 0; i < numb_models; i++)
		//{
		//	model_list.push_back(list[i]); // Adding models to list
		//}
	}

	void draw_cursor(Shader shader, Heightmap hm, float camx, float camy, float camz)
	{
		glm::mat4 curr_model = glm::mat4(1.0f);
		if (PLACEABLE_YSTRAIN)
		{
			curr_model = glm::translate(curr_model, glm::vec3(camx, hm.grab_height(camx, camz), camz));
		}
		else
		{
			curr_model = glm::translate(curr_model, glm::vec3(camx, camy, camz));

		}
		curr_model = glm::scale(curr_model, glm::vec3(PLACEABLE_SCALE, PLACEABLE_SCALE, PLACEABLE_SCALE));	// Scale
		curr_model = glm::rotate(curr_model, glm::radians((float)PLACEABLBE_ROTATION), glm::vec3((int)PLACEABLE_ROT_X, (int)PLACEABLE_ROT_Y, (int)PLACEABLE_ROT_Z));
		

		shader.use();
		shader.setMat4("model", curr_model);
		model_list[PLACEABLE_MENU_INDEX].Draw(shader);

		steady_model_mat = curr_model;
	}

	void place()
	{
		model_matrix.push_back(steady_model_mat);
		index_stack.push_back(PLACEABLE_MENU_INDEX);
	}

	void draw(Shader shader)
	{
		if (!index_stack.empty())
		{
			for (unsigned int i = 0; i < index_stack.size(); i++)
			{
				//std::cout << index_stack[i];
				shader.setMat4("model", model_matrix[i]);
				model_list[index_stack[i]].Draw(shader);
			}
		}
	}
};