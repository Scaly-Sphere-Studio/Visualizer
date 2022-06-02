#pragma once
#include <SSS/GL.hpp>
#include <vector>
#include "commons.h"
#include "shader.hpp"
#include "visualizer.h"
#include <cmath>
#include <random>
#include <time.h>

#include <SSS/Line/line.h>


struct debug_Vertex {
	debug_Vertex(float x, float y, float z, glm::vec3 col);
	float _x, _y, _z;
	glm::vec3 _col;
};

struct testBox {
	testBox(glm::vec3 _pos, glm::vec2 s, glm::vec4 _col);
	glm::vec3 pos;
	glm::vec2 size;
	glm::vec4 color;
};

class Box
{
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


	// DATA
	std::string text;
	std::string id;
	std::vector<uint16_t> tags;

	static std::vector<testBox> box_batch;
	static GLuint box_shader;

	std::vector<std::string> link_to;
	std::vector<std::string> link_from;

};


class Visualizer {
public:
	Visualizer();
	~Visualizer();

	void run();

private:

	void draw();
	void input();
	void debug_show(GLuint buffer, void* data, size_t size);

	glm::vec3 clear_color = glm::vec3{ 1.0f };
	std::vector<Box> boxes;
	std::unordered_map<std::string, Box> box_map;
	
	
	
	
	
	void setup();

	GLFWwindow* window;
	GLuint vertexbuffer;
	double c_x = 0.0, c_y = 0.0;
	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 mvp;


	GLuint VertexArrayID;

	GLuint MatrixID;

	float w_h = 700;
	float w_w = 900;
	glm::vec3 cam_pos{ 0,0,3 };

	//DEBUG
	GLuint debug_vb;
	std::vector<debug_Vertex> debug_batch;
	void debug_box(Box& b);

	//Differents shapes for the debugging process
	void circle(float x, float y, float z, float radius);
	void square(float x, float y, float radius);
	void cross(float x, float y, float radius, float angle = 0);
	void rectangle(float x, float y, float width, float height);

	//Check if the box is on the screen
	bool check_frustrum_render(Box &b);
	void frustrum_test();
	void drag_boxes();

	//Shaders
	GLuint programID;
	GLuint debugID;
	GLuint line_shader_ID;

	//Instancing
	GLuint billboard_vertex_buffer;
	GLuint particles_data;



};
