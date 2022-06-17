#pragma once

#include "Box.h"

struct debug_Vertex {
	debug_Vertex(float x, float y, float z, glm::vec3 col);
	float _x, _y, _z;
	glm::vec3 _col;
};

class Debugger
{
public:
	Debugger();
	~Debugger();
	//DEBUG
	GLuint debug_vb;
	GLuint debugID;

	bool debugmode = true;


	std::vector<debug_Vertex> debug_batch;
	void debug_box(const Box& b);
	void debug_show(GLuint buffer, void* data, size_t size);

	//Differents shapes for the debugging process
	void circle(float x, float y, float z, float radius);
	void square(float x, float y, float radius);
	void cross(float x, float y, float radius, float angle = 0);
	void rectangle(float x, float y, float width, float height);
};

