#pragma once

#include "commons.h"
#include "shader.hpp"

struct testBox {
	testBox();
	testBox(glm::vec3 _pos, glm::vec2 s, glm::vec4 _col);
	glm::vec3 _pos;
	glm::vec2 _size;
	glm::vec4 _color;
};


class Box : public testBox{
public:
	Box(glm::vec3 _pos, glm::vec2 _s, std::string hex = "000000");
	~Box();

	void set_selected_col(std::string hex);
	void set_col(std::string hex);


	bool check_collision(glm::vec3 const& c_pos);


	//STATES 
	bool _render = true;
	bool _hovered = false;
	bool _clicked = false;


	//Box rendering
	std::vector<testBox> model;


	glm::vec3 selected_color = glm::vec3(0.93f, 0.64f, 0.43f);

	void create_box();
	//Update the positions of all the subboxes 
	void update();


	//Return the coordinates of the center of the box
	glm::vec3 center();

	// DATA
	std::string text;
	std::string id;
	std::vector<uint16_t> tags;

	static std::vector<testBox> box_batch;


	std::list<std::string> link_to;
	std::list<std::string> link_from;

};


class Selection_square : testBox {
public:

	//Box rendering
	testBox model;

	static std::vector<testBox> box_batch;
};


static bool sort_box(testBox& a, testBox& b) {

	return a._pos.z < b._pos.z;
}
