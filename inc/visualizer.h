#pragma once

#include "Box.h"
#include "Debugger.h"
#include "backend.hpp"

enum struct V_STATES {
	DEFAULT = 0,
	CUTLINE = 1,
	MULTI_SELECT = 2
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
	//friend void to_json(nlohmann::json& j, const Visualizer& t);
	//friend void from_json(const nlohmann::json& j, Visualizer& t);

	//CALLBACKS
	static void resize_callback(GLFWwindow* win, int w, int h);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	void setup();
	void input();
	void refresh();
	
	V_STATES _states = V_STATES::DEFAULT;

	glm::vec3 clear_color = glm::vec3{ 1.0f };
	//std::unordered_map<std::string, Box> box_map;
	std::unordered_map<std::string, SSS::GL::Polyline::Shared> arrow_map;


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
	
	std::string current_selected_ID;
	std::string last_selected_ID;
	std::string first_link_ID;
	std::string second_link_ID;

	//Translate the screen cursor position from input to the world coordinates
	glm::vec3 cursor_map_coordinates();

	/* [VISUALIZER METHODS] */
	//Check if the box is on the screen
	bool check_frustrum_render(Box &b);
	void frustrum_test();
	void drag_boxes();
	void line_drag_link();
	void multi_select_drag();
	std::string clicked_box_ID(std::string &ID);


	/* [RENDERER DATA] */
	// Objects
	uint32_t main_cam_id{ 0 };
	// Renderers
	uint32_t line_renderer_id{ 0 };
	uint32_t box_renderer_id{ 0 };
	uint32_t debug_renderer_id{ 0 };

	SSS::GL::Window::Shared window;
	double c_x = 0.0, c_y = 0.0;


	//PARSER
	void parse_info_data_visualizer_from_json(const std::string& path);
	void parse_info_data_visualizer_to_json(const std::string& path, const bool prettify = false);
	
	void parse_info_data_project_from_json(const std::string& path);
	void parse_info_data_project_to_json(const std::string& path, const bool prettify = false);
	
	void save();
	void load();

	Tags test_tag;

};

//JSON CONVERTION
//Text data convertion
void to_json(nlohmann::json& j, const VISUALISER_INFO& t);
void from_json(const nlohmann::json& j, VISUALISER_INFO& t);

void to_json(nlohmann::json& j, const PROJECT_DATA& t);
void from_json(const nlohmann::json& j, PROJECT_DATA& t);
