#pragma once
#include <SSS/GL.hpp>
#include <vector>
#include "commons.h"
#include "shader.hpp"
#include "visualizer.h"
#include <cmath>



struct Vertex {
	Vertex(float x, float y, float z, glm::vec3 col);
	float _x, _y, _z;
	glm::vec3 _col;
};

class Box
{
public:
	Box(float width, float height, std::string hex = "000000");
	~Box();

	std::vector<Vertex> model;
	void log();
	void set_selected_col(std::string hex);
	void set_col(std::string hex);
	void update();

	bool check_collision(double x, double y);

	bool render = true;
	float _h = 150.0f, _w = 300.0f;
	bool _selected = false;


	glm::vec3 pos = glm::vec3(0, 0, 0);
	glm::vec3 base_color = glm::vec3(0, 0, 0);
	glm::vec3 selected_color = glm::vec3(0.93f, 0.64f, 0.43f);

	void create_box();

};


class Visualizer {
public:
	Visualizer();
	~Visualizer();

	void run();

private:

	void draw(GLuint buffer, void* data, size_t size);
	void input();
	void debug_show(GLuint buffer, void* data, size_t size);

	glm::vec3 clear_color = glm::vec3{ 1.0f };
	std::vector<Box> boxes;
	std::vector<Vertex> batch;
	

	GLFWwindow* window;
	GLuint vertexbuffer;
	double c_x = 0.0, c_y = 0.0;
	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 mvp;

	GLuint MatrixID;

	int w_h = 700;
	int w_w = 900;
	glm::vec3 cam_pos{ 0,0,3 };

	//DEBUG
	GLuint debug_vb;
	std::vector<Vertex> debug_batch;

	//Differents shapes for the debugging process
	void circle(float x, float y, float radius);
	void square(float x, float y, float radius);
	void cross(float x, float y, float radius, float angle = 0);
	void rectangle(float x, float y, float width, float height);

	//Check if the box is on the screen
	bool check_frustrum_render(Box &b);
	
};
