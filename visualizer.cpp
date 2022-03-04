#include "visualizer.h"

Box::Box(float width, float height)
{
	create_box();
}

Box::~Box()
{
}

void Box::log()
{
	for (Vertex v : model) {
		std::cout << v._x << " ; " << v._y << " ; " << v._z << std::endl;
	}
}

void Box::create_box()
{

	model.emplace_back(pos.x, pos.y, 0.0f);
	model.emplace_back(pos.x, pos.y - _h, 0.0f);
	model.emplace_back(pos.x + _w, pos.y - _h, 0.0);

	model.emplace_back(pos.x + _w, pos.y - _h, 0.0);
	model.emplace_back(pos.x + _w, pos.y, 0.0);
	model.emplace_back(pos.x, pos.y, 0.0);
}

Vertex::Vertex(float x, float y, float z)
{
	_x = x;
	_y = y;
	_z = z;
}
