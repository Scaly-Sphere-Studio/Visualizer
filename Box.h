#pragma once

#include <SSS/GL.hpp>
#include <vector>
#include "commons.h"
#include "shader.hpp"
#include <cmath>
#include <random>
#include <time.h>


#include <SSS/Line/line.h>

#include <unordered_map>


struct testBox {
	testBox(glm::vec3 _pos, glm::vec2 s, glm::vec4 _col);
	glm::vec3 pos;
	glm::vec2 size;
	glm::vec4 color;
};


class Box {
public:
	Box(float width, float height, std::string hex = "000000");
	~Box();

	void set_selected_col(std::string hex);
	void set_col(std::string hex);


	bool check_collision(double x, double y);


	float _h = 150.0f, _w = 300.0f;


	//STATES 
	bool _render = true;
	bool _hovered = false;
	bool _clicked = false;


	//Box rendering
	std::vector<testBox> model;

	glm::vec3 pos = glm::vec3(0, 0, 0);
	glm::vec3 base_color = glm::vec3(0, 0, 0);
	glm::vec3 selected_color = glm::vec3(0.93f, 0.64f, 0.43f);

	void create_box();
	void update();
	glm::vec3 center();

	// DATA
	std::string text;
	std::string id;
	std::vector<uint16_t> tags;

	static std::vector<testBox> box_batch;
	static GLuint box_shader;

	std::vector<std::string> link_to;
	std::vector<std::string> link_from;

};


static bool sort_box(testBox& a, testBox& b) {

	return a.pos.z < b.pos.z;
}
