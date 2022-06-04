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
	/*std::vector<Box> boxes;*/
	std::unordered_map<std::string, Box> box_map;
	std::unordered_map<std::string, std::shared_ptr<Polyline>> arrow_map;
	
	void link_box(const Box& a, const Box& b);
	
	

	GLFWwindow* window;
	GLuint vertexbuffer;
	double c_x = 0.0, c_y = 0.0;
	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 mvp;


	GLuint VertexArrayID;

	GLuint MatrixID;

	float w_h = 810;
	float w_w = 1440;
	glm::vec3 cam_pos{ 0,0,3 };

	


	//Check if the box is on the screen
	bool check_frustrum_render(Box &b);
	void frustrum_test();
	void drag_boxes();

	//Shaders
	GLuint programID;
	
	GLuint line_shader_ID;

	//Instancing
	GLuint billboard_vertex_buffer;
	GLuint particles_data;


	Debugger debug;
};


