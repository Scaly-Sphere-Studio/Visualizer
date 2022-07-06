#pragma once

#include "Box.h"

struct debug_Vertex {
	debug_Vertex(float x, float y, float z, glm::vec3 col);
	float _x, _y, _z;
	glm::vec3 _col;
};

class Debugger : public SSS::GL::Renderer {
	friend SSS::GL::Window;

private:
	Debugger(std::weak_ptr<SSS::GL::Window> win, uint32_t id);

public:
	~Debugger();
	
	virtual void render();

private:
	SSS::GL::Basic::VBO::Ptr vbo;

	//DEBUG
	std::vector<debug_Vertex> debug_batch;
	void debug_box(const Box& b);

	//Differents shapes for the debugging process
	void circle(float x, float y, float z, float radius);
	void square(float x, float y, float radius);
	void cross(float x, float y, float radius, float angle = 0);
	void rectangle(float x, float y, float width, float height);
};
