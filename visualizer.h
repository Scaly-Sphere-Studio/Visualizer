#pragma once
#include <SSS/GL.hpp>
#include <vector>
#include "commons.h"
#include "shader.hpp"
#include "Debugger.h"
#include <cmath>
#include <random>
#include <time.h>

#include "Box.h"

#include <SSS/Line/line.h>

#include <unordered_map>


class Visualizer {
public:
	Visualizer();
	~Visualizer();

	void run();

private:
	void setup();
	void draw();
	void input();
	

	glm::vec3 clear_color = glm::vec3{ 1.0f };
	std::unordered_map<std::string, Box> box_map;
	std::unordered_map<std::string, std::shared_ptr<Polyline>> arrow_map;
	

	//Link with an arrow the box a to the box b, and add the ID in their linked ID list
	void link_box(Box& a, Box& b);
	//Update all the arrow linked to this box
	void link_box(Box& a);
	void link_box_to_cursor(Box& a);
	//Add a new box
	void push_box(std::string boxID);
	//Remove the selected box
	void pop_box(std::string ID);
	
	std::string current_selected_ID = "";
	std::string last_selected_ID = "";
	
	std::string first_link_ID = "";
	std::string second_link_ID = "";



	int e_count = 0;




	std::string i1 = rand_color();
	std::string i2 = rand_color();

	GLuint VertexArrayID;

	GLuint MatrixID;

	float w_h = 810;
	float w_w = 1440;
	glm::vec3 cam_pos{ 0,0,3 };

	


	//Check if the box is on the screen
	bool check_frustrum_render(Box &b);
	void frustrum_test();
	void drag_boxes();

	void line_drag_link();

	std::string clicked_box_ID(std::string &ID);

	//GL CONTEXT / SHADERS / PARTICLES
	//Shaders
	GLuint programID;
	GLuint line_shader_ID;

	//Instancing
	GLuint billboard_vertex_buffer;
	GLuint particles_data;

	GLFWwindow* window;
	GLuint vertexbuffer;
	double c_x = 0.0, c_y = 0.0;
	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 mvp;

	Debugger debug;
};
