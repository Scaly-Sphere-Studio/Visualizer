#pragma once
#include <SSS/GL.hpp>
#include <vector>
#include "commons.h"
#include "shader.hpp"
#include "visualizer.h"



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
private:
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
	glm::vec3 clear_color = glm::vec3{ 1.0f };
	Box box{ 0.0f , 0.0f, "FFCFD2"};
	GLFWwindow* window;
	GLuint vertexbuffer;

	double c_x = 0.0, c_y = 0.0;
	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 mvp;

	GLuint MatrixID;

	int w_h = 480;
	int w_w = 640;

	glm::vec3 cam_pos{ 0,0,3 };
	
};
