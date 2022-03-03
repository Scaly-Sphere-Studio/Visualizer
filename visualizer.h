#pragma once
#include <SSS/GL.hpp>
#include <vector>


struct Vertex {
	Vertex(float x, float y, float z);
	float _x, _y, _z;
};

class Box
{
public:
	Box(float width, float height);
	~Box();

	std::vector<Vertex> model;
private:
	int _h, _w;
	bool _selected;
	
	glm::vec3 pos = glm::vec3(0, 0, 0);
	glm::vec4 base_color = glm::vec4(0, 0, 0, 1);
	glm::vec4 slected_color = glm::vec4(1, 1, 1, 1);	

	void create_box();
};

