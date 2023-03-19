#pragma once

#include "Box.h"

struct debug_Vertex {
	debug_Vertex(float x, float y, float z, glm::vec3 col);
	float _x, _y, _z;
	glm::vec3 _col;
};

class Debugger : public SSS::GL::Renderer<Debugger> {
	friend SSS::GL::Basic::SharedBase<Debugger>;
	friend SSS::GL::Window;

private:
	Debugger(SSS::GL::Window::Shared win);

public:
	~Debugger();
	
	SSS::GL::Camera::Shared camera;

	virtual void render();

private:
	SSS::GL::Basic::VBO vbo;

	//DEBUG
	std::vector<debug_Vertex> debug_batch;
	void debug_box(const Box& b);

	//Differents shapes for the debugging process
	void circle(float x, float y, float z, float radius);
	void square(float x, float y, float radius);
	void cross(float x, float y, float radius, float angle = 0);
	void rectangle(float x, float y, float width, float height);
};
