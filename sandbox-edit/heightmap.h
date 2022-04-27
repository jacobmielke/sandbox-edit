#pragma once
#include "shader.h"
#include "camera.h"
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
	const char* filenames[5]{ "heightmaps/32x32.png" ,"heightmaps/128x128.png", "heightmaps/512x512.png", "heightmaps/small.png", "heightmaps/512x512_blank.png"};
	const char* texture_names[3]{ "textures/512x512/Dirt/Dirt_18-512x512.png","textures/512x512/Grass/Grass_08-512x512.png","textures/512x512/Stone/Stone_05-512x512.png" }; // Hillside
	//const char* texture_names[3]{ "textures/512x512/snow.jpg","textures/512x512/snow.jpg","textures/512x512/Stone/Stone_05-512x512.png" }; // Snow
	//const char* texture_names[3]{ "textures/512x512/Dirt/Dirt_14-512x512.png","textures/512x512/Dirt/Dirt_12-512x512.png","textures/512x512/Stone/Stone_03-512x512.png" }; // Desert
	Heightmap(const char* path);

	void reload_heightmap(const char* path);
	void export_heightmap(int w, int h, int nC);
	void recreate_custom(std::string filename);
	void draw(Shader& shader);
	void scan_heightmap(float mouse_x, float mouse_z, Camera camera);
	void activate_textures(Shader shader);
	float grab_height(float x, float z);
	

private:
	void load_heightmap(const char* filename, int* w, int* h, int* nC);
	void process_heightmap();
	void load_texture();
};