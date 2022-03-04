#pragma once
#include <SSS/GL.hpp>
#include <vector>
#include "commons.h"


struct Vertex {
	Vertex(float x, float y, float z, glm::vec3 col);
	float _x, _y, _z;
	glm::vec3 _col;
};

class Box
{
public:
	Box(float width, float height, std::string hex = "FFFFFF");
	~Box();

	std::vector<Vertex> model;
	void log();
	void set_selected_col(std::string hex);
	void set_col(std::string hex);
private:
	float _h = 150.0f, _w = 150.0f;
	bool _selected;
	
	glm::vec3 pos = glm::vec3(0, 0, 0);
	glm::vec3 base_color = glm::vec3(0, 0, 0);
	glm::vec3 selected_color = glm::vec3(1, 1, 1);	

	void create_box();
};

