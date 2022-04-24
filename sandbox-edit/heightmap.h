#pragma once
#include <glad/glad.h> 
#include "stb_image.h"
#include <vector>
#include <iostream>
#include <string>
#include "shader.h"
#include "options.h"
class Heightmap
{
public:
	int width, height, nChannels, numStrips, numTrisPerStrip, res = 1;
	unsigned int terrainVAO, terrainVBO, terrainIBO;
	unsigned char* data;
	unsigned int texture[3];
	int t_width, t_height, t_nrChannels;
	std::vector<float> vertices;
	std::vector<unsigned> indices;
	std::vector<glm::vec2> uv;

	Heightmap(const char* path)
	{
		load_heightmap(path, &width, &height, &nChannels);
		process_heightmap();
		load_texture();
	}

	void draw(Shader& shader)
	{
		glBindVertexArray(terrainVAO);

		// Bind Textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[0]);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture[1]);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texture[2]);

		for (unsigned strip = 0; strip < numStrips; strip++)
		{
			glDrawElements(GL_TRIANGLE_STRIP, numTrisPerStrip + 2, GL_UNSIGNED_INT, (void*)(sizeof(unsigned) * (numTrisPerStrip + 2) * strip));
		}

		glActiveTexture(GL_TEXTURE0);
	}

	void scan_heightmap(float mouse_x, float mouse_z, Camera camera)
	{
		for (int i = 0; i < vertices.size(); i += 3)
		{
			if( (mouse_x >= vertices[i] - TOOL_RADIUS && mouse_x <= vertices[i] + TOOL_RADIUS) && (mouse_z >= vertices[i+2] - TOOL_RADIUS && mouse_z <= vertices[i+2] + TOOL_RADIUS) )
			{
				glm::vec2 curr_vert = glm::vec2(vertices[i], vertices[i + 2]); // Current vert position
				glm::vec2 dist_vec = glm::vec2(camera.w_xpos, camera.w_zpos) - curr_vert;
				float mag = std::sqrt(dist_vec.x * dist_vec.x + dist_vec.y * dist_vec.y); // Distance between tool and vert

				vertices[i + 1] += CURRENT_TOOL * (TOOL_INTENSITY / (mag * TOOL_OPACITY));
			}
		}
		glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), &vertices[0]);
	}

	void activate_textures(Shader shader)
	{
		shader.use();
		shader.setInt("texture1", 0);
		shader.setInt("texture2", 1);
		shader.setInt("texture3", 2);
	}

	float grab_height(float x, float z)
	{
		float tol = 0.2f;
		for (int i = 0; i < vertices.size(); i += 3)
		{
			if ((x >= vertices[i] - tol && x <= vertices[i] + tol) && (z >= vertices[i + 2] - tol && z <= vertices[i + 2] + tol))
			{
				return vertices[i + 1];
			}
		}
	}

private:
	void load_heightmap(const char* filename, int* w, int* h, int* nC)
	{
		data = stbi_load(filename, w, h, nC, 0);
		if (data)
		{
			std::cout << "Loaded heightmap of size " << height << " x " << width << std::endl;
		}
		else
		{
			std::cout << "Failed to load heigtmap" << std::endl;
		}

	}

	void process_heightmap()
	{
		// Vertices
		float yScale = 64.0f / 256.0f, yShift = 16.0f;
		unsigned bytePerPixel = nChannels;
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				unsigned char* pixelOffset = data + (j + width * i) * bytePerPixel;
				unsigned char y = pixelOffset[0];

				// Each Vetex
				vertices.push_back(-height / 2.0f + height * i / (float)height);   // vx
				vertices.push_back((int)y * yScale - yShift);   // vy
				vertices.push_back(-width / 2.0f + width * j / (float)width);   // vz
			}
		}
		std::cout << "Loaded " << vertices.size() / 3 << " vertices" << std::endl;
		stbi_image_free(data);

		// Indices
		for (unsigned i = 0; i < height - 1; i += res)
		{
			for (unsigned j = 0; j < width; j += res)
			{
				for (unsigned k = 0; k < 2; k++)
				{
					indices.push_back(j + width * (i + k * res));
				}
			}
		}

		std::cout << "Loaded " << indices.size() << " indices" << std::endl;
		numStrips = (height - 1) / res;
		numTrisPerStrip = (width / res) * 2 - 2;
		std::cout << "Created lattice of " << numStrips << " strips with " << numTrisPerStrip << " triangles each" << std::endl;
		std::cout << "Created " << numStrips * numTrisPerStrip << " triangles total" << std::endl;

		// OpenGL buffer Setup
		glGenVertexArrays(1, &terrainVAO);
		glBindVertexArray(terrainVAO);
		std::cout << "Made Vertex" << std::endl;
		glGenBuffers(1, &terrainVBO);
		glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);
		std::cout << "Made VBO" << std::endl;
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		std::cout << "Made Position" << std::endl;
		glGenBuffers(1, &terrainIBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), &indices[0], GL_DYNAMIC_DRAW);
		std::cout << "Made IBO" << std::endl;
	}

	void load_texture()
	{
		// RGB mode
		GLenum format = GL_RGB;
		// Grass Texture
		glGenTextures(1, &texture[0]);
		glBindTexture(GL_TEXTURE_2D, texture[0]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Filters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);


		unsigned char* data = stbi_load("textures/512x512/Grass/Grass_08-512x512.png", &t_width, &t_height, &t_nrChannels, 0);
		if (data)
		{
			if (t_nrChannels == 1)
				format = GL_RED;
			else if (t_nrChannels == 3)
				format = GL_RGB;
			else if (t_nrChannels == 4)
				format = GL_RGBA;

			glTexImage2D(GL_TEXTURE_2D, 0, format, t_width, t_height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			std::cout << "Loaded terrain texture " << t_width << "x" << t_height << std::endl;

		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);

		glGenTextures(1, &texture[1]);
		glBindTexture(GL_TEXTURE_2D, texture[1]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		data = stbi_load("textures/512x512/Grass/Grass_19-512x512.png", &t_width, &t_height, &t_nrChannels, 0);
		if (data)
		{
			if (t_nrChannels == 1)
				format = GL_RED;
			else if (t_nrChannels == 3)
				format = GL_RGB;
			else if (t_nrChannels == 4)
				format = GL_RGBA;

			glTexImage2D(GL_TEXTURE_2D, 0, format, t_width, t_height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			std::cout << "Loaded terrain texture " << t_width << "x" << t_height << std::endl;

		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);

		// Rock Texture
		glGenTextures(1, &texture[2]);
		glBindTexture(GL_TEXTURE_2D, texture[2]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Filters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		data = stbi_load("textures/512x512/Stone/Stone_06-512x512.png", &t_width, &t_height, &t_nrChannels, 0);
		if (data)
		{
			if (t_nrChannels == 1)
				format = GL_RED;
			else if (t_nrChannels == 3)
				format = GL_RGB;
			else if (t_nrChannels == 4)
				format = GL_RGBA;

			glTexImage2D(GL_TEXTURE_2D, 0, format, t_width, t_height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			std::cout << "Loaded terrain texture " << t_width << "x" << t_height << std::endl;

		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);
	}
};