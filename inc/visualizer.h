#pragma once

#include "Box.h"
#include "Debugger.h"
#include "backend.hpp"

enum struct V_STATES {
	DEFAULT = 0,
	CUTLINE = 1,
	DRAG_BOX = 2,
	MULTI_SELECT = 3,
	CONNECT_LINE = 4
};

struct VISUALISER_INFO {
	float _h{ 810.f };
	float _w{ 1440.f };
};

struct PROJECT_DATA {
	~PROJECT_DATA();
	std::unordered_map<std::string, Box> box_map;
};

class Visualizer {
	friend Debugger;

public:
	// Make constructor public for JSON operations
	Visualizer();
	using Ptr = std::unique_ptr<Visualizer>;
	static Ptr const& get();
	~Visualizer();

	void run();

private:
	PROJECT_DATA _proj;
	VISUALISER_INFO _info;
	V_STATES _states = V_STATES::DEFAULT;

	//CALLBACKS
	static void resize_callback(GLFWwindow* win, int w, int h);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouse_callback(GLFWwindow* window, int button, int action, int mods);

	void setup();
	void input();
	void refresh();
	void save();
	void load();


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
	//Translate the screen cursor position from input to the world coordinates
	glm::vec3 cursor_map_coordinates();

	

	std::set<std::string> _selected_IDs;
	Particle Selection_box{glm::vec3(INT_MAX,INT_MAX,0), glm::vec2(50,50), glm::vec4(0,0,0,0)};


	std::string first_link_ID;
	std::string second_link_ID;
	glm::vec3 _cur_pos;
	glm::vec3 _otherpos;

	/* [VISUALIZER METHODS] */
	//Check if the box is on the screen
	bool check_frustrum_render(Box &b);
	void frustrum_test();
	void drag_boxes();
	void cut_link_line();
	void connect_drag_line();
	void multi_select();
	//Check if the user clicked on a box and returns its ID
	std::string clicked_box_ID(std::string &ID);

	/* [RENDERER DATA] */
	// Objects
	SSS::GL::Camera::Shared camera;
	// Renderers
	uint32_t line_renderer_id{ 0 };
	uint32_t box_renderer_id{ 0 };
	uint32_t debug_renderer_id{ 0 };

	SSS::GL::Window::Shared window;
	double c_x = 0.0, c_y = 0.0;
	int mod = INT_MAX;
	int mouse_action = INT_MAX;

	glm::vec3 clear_color = glm::vec3{ 1.0f };
	std::unordered_map<std::string, SSS::GL::Polyline::Shared> arrow_map;
	Traduction_data _td;

	//PARSER
	void parse_info_data_visualizer_from_json(const std::string& path);
	void parse_info_data_visualizer_to_json(const std::string& path, const bool prettify = false);
	
	void parse_info_data_project_from_json(const std::string& path);
	void parse_info_data_project_to_json(const std::string& path, const bool prettify = false);

	//Check if a double click is detected in a selected time interval
	bool double_click_detection(std::chrono::milliseconds timestamp);
};

//JSON CONVERTION
//Text data convertion
void to_json(nlohmann::json& j, const VISUALISER_INFO& t);
void from_json(const nlohmann::json& j, VISUALISER_INFO& t);

void to_json(nlohmann::json& j, const PROJECT_DATA& t);
void from_json(const nlohmann::json& j, PROJECT_DATA& t);
