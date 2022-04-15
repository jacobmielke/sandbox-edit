#pragma once
#include <glad/glad.h> 
#include "stb_image.h"
#include <vector>
#include <iostream>
#include <string>
#include "shader.h"

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


		//        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		for (unsigned strip = 0; strip < numStrips; strip++)
		{
			glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
				numTrisPerStrip + 2,   // number of indices to render
				GL_UNSIGNED_INT,     // index data type
				(void*)(sizeof(unsigned) * (numTrisPerStrip + 2) * strip)); // offset to starting index

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

				// vertex
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
		// first, configure the cube's VAO (and terrainVBO + terrainIBO)
		glGenVertexArrays(1, &terrainVAO);
		glBindVertexArray(terrainVAO);
		std::cout << "Made Vertex" << std::endl;
		glGenBuffers(1, &terrainVBO);
		glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
		std::cout << "Made VBO" << std::endl;
		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		std::cout << "Made Position" << std::endl;
		glGenBuffers(1, &terrainIBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainIBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), &indices[0], GL_STATIC_DRAW);
		std::cout << "Made IBO" << std::endl;
	}

	void load_texture()
	{
		// Grass Texture
		glGenTextures(1, &texture[0]);
		glBindTexture(GL_TEXTURE_2D, texture[0]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
		// Loop if pass 1
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
		// Filters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		// load image, create texture and generate mipmaps

		unsigned char* data = stbi_load("textures/512x512/Grass/Grass_08-512x512.png", &t_width, &t_height, &t_nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			std::cout << "Loaded terrain texture " <<t_width << "x" << t_height << std::endl;
				
		}
		else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
		stbi_image_free(data);

		// Mid Texture
		glGenTextures(1, &texture[1]);
		glBindTexture(GL_TEXTURE_2D, texture[1]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
		// Loop if pass 1
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Filters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		// load image, create texture and generate mipmaps

		data = stbi_load("textures/512x512/Dirt/Dirt_02-512x512.png", &t_width, &t_height, &t_nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
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
		// Loop if pass 1
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// Filters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		// load image, create texture and generate mipmaps

		data = stbi_load("textures/512x512/Stone/Stone_06-512x512.png", &t_width, &t_height, &t_nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
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