#pragma once

#include "Box.h"
#include "Debugger.h"

enum struct V_STATES {
	DEFAULT = 0,
	CUTLINE = 1,
	MULTI_SELECT = 2
};

class Visualizer {
	Visualizer();
public:
	using Ptr = std::unique_ptr<Visualizer>;
	static Ptr const& get();
	~Visualizer();

	void run();

private:
	static void resize_callback(GLFWwindow* win, int w, int h);


	void setup();
	void draw();
	void input();
	
	V_STATES _states = V_STATES::DEFAULT;
	float w_h{ 810.f };
	float w_w{ 1440.f };
	glm::vec3 clear_color = glm::vec3{ 1.0f };
	std::unordered_map<std::string, Box> box_map;
	std::unordered_map<std::string, std::shared_ptr<Polyline>> arrow_map;
	std::string i1 = rand_color();
	std::string i2 = rand_color();
	

	/* [BOX METHODS] */
	//Link with an arrow the box a to the box b, and add the ID in their linked ID list
	void link_box(Box& a, Box& b);
	//Update all the arrow linked to this box
	void link_box(Box& a);
	//Create a link from the box to the position of the cursor
	void link_box_to_cursor(Box& a);
	//Remove the link between two selected box
	void pop_link(Box& a, Box& b);

	//Add a new box at the current cursor position
	void push_box(std::string boxID);
	//Remove the selected box
	void pop_box(std::string ID);
	
	std::string current_selected_ID = "";
	std::string last_selected_ID = "";
	std::string first_link_ID = "";
	std::string second_link_ID = "";




	/* [VISUALIZER METHODS] */
	//Check if the box is on the screen
	bool check_frustrum_render(Box &b);
	void frustrum_test();
	void drag_boxes();
	void line_drag_link();
	void multi_select_drag();
	std::string clicked_box_ID(std::string &ID);


	/* [RENDERER DATA] */
	GLuint VertexArrayID;
	GLuint MatrixID;
	glm::vec3 cam_pos{ 0,0,3 };
	//GL CONTEXT / SHADERS / PARTICLES
	//Shaders
	GLuint programID;
	GLuint line_shader_ID;

	//Instancing
	GLuint billboard_vertex_buffer;
	GLuint particles_data;

	SSS::GL::Window::Shared window;
	GLuint vertexbuffer;
	double c_x = 0.0, c_y = 0.0;

	void _updateProj();
	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 mvp;

	/* [DEBUGGER] */
	Debugger debug;

	/* [MISC] */
	bool cubic_bezier_segment_intersection(glm::vec3 b_a, glm::vec3 b_b, glm::vec3 b_c, glm::vec3 b_d, glm::vec3 s_a, glm::vec3 s_b);
	std::array<float, 3> CubicRoots(float a, float b, float c, float d);
	std::array<float, 4> BezierCoeffs(float P0, float P1, float P2, float P3);
};
