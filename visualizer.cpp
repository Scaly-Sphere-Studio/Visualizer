#include "visualizer.h"

Box::Box(float width, float height, std::string hex)
{
	base_color = hex_to_rgb(hex);
	create_box();
}

Box::~Box()
{
	model.clear();
}

void Box::log()
{
	for (Vertex v : model) {
		std::cout << v._x << " ; " << v._y << " ; " << v._z << std::endl;
	}
}

void Box::set_selected_col(std::string hex)
{
	selected_color = hex_to_rgb(hex);
}

void Box::set_col(std::string hex)
{
	base_color = hex_to_rgb(hex);
}

void Box::create_box()
{
	model.emplace_back(pos.x, pos.y, 0.0f, base_color);
	model.emplace_back(pos.x, pos.y - _h, 0.0f, base_color);
	model.emplace_back(pos.x + _w, pos.y - _h, 0.0, base_color);

	model.emplace_back(pos.x + _w, pos.y - _h, 0.0, base_color);
	model.emplace_back(pos.x + _w, pos.y, 0.0, base_color);
	model.emplace_back(pos.x, pos.y, 0.0, base_color);
}

Vertex::Vertex(float x, float y, float z, glm::vec3 col)
{
	_x = x;
	_y = y;
	_z = z;

	_col = col;
}
