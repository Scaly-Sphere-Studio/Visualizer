#pragma once

#include "commons.h"
#include "Text_data.h"

#define epsilon 1.0f / static_cast<float>(RAND_MAX * 100.0f)

struct Particle {
	Particle();
	Particle(glm::vec3 pos, glm::vec2 s, glm::vec4 col);
	Particle(std::string t, const SSS::TR::Format &fmt, glm::vec3 pos, glm::vec2 s);
	// ---------- Below data is passed to OpenGL VBO
	glm::vec3 _pos;
	glm::vec2 _size;
	glm::vec4 _color;
	GLuint _glsl_tex_unit{ UINT32_MAX };
	// ---------- Below data is purely internal and not passed to OpenGL
	uint32_t _sss_tex_id{ UINT32_MAX };

	//Return the coordinates of the center of the box
	glm::vec3 center();
	//Check for a collision box/point
	bool check_collision(Particle p);
	//Check for a collision with another particle
	bool check_collision(glm::vec3 const& c_pos);
};

struct Tags : public Particle {
	Tags();
	Tags(std::string _name, std::string hex = "#FFFFFF", uint32_t weight = 1);
	~Tags();
	std::string name;
	std::vector<Particle> model;
	uint32_t weight;
};


class Box : public Particle{
public:
	Box();
	Box(glm::vec3 _pos, glm::vec2 _s, std::string hex = "000000");
	~Box();
	
	//Update the position with a delta for dragging the boxes
	void update_pos(glm::vec3 delta);
	void update();

	void set_selected_col(std::string hex);
	void set_col(std::string hex);

	//STATES 
	bool _render = true;
	bool _hovered = false;
	bool _clicked = false;
	glm::vec3 selected_color = glm::vec3(0.93f, 0.64f, 0.43f);

	//Box rendering
	
	//Initialisation of the box and fill the model array
	void create_box();
	//Update the positions of all the subboxes 
	int check_text_selection(glm::vec3 const& c_pos);

	// DATA
	Text_data _td;   

	std::string _id;
	std::vector<Particle> model;
	std::vector<Particle> text_model;

	std::vector<uint16_t> tags;
	std::set<std::string> link_to;
	std::set<std::string> link_from;
	
	static std::vector<Particle> box_batch;
	static std::map<uint16_t, Tags> tags_list;
};

class Selection_square : Particle {
public:

	//Box rendering
	Particle model;

	static std::vector<Particle> box_batch;
};


class Text_particle : Particle {
public:
	Text_particle(glm::vec3 _pos, glm::vec2 _s, const std::string text, const SSS::TR::Format& fmt);
	//Box rendering
	Particle model;

};



class BoxRenderer : public SSS::GL::Renderer {
	friend SSS::GL::Window;

private:
	BoxRenderer(std::weak_ptr<SSS::GL::Window> win, uint32_t id);

public:
	SSS::GL::Camera::Shared camera;

	virtual void render();

private:
	struct Batch {
		GLuint offset{ 0 };
		GLuint count{ 0 };
		std::vector<uint32_t> tex_ids;
	};

	SSS::GL::Basic::VAO vao;
	SSS::GL::Basic::VBO billboard_vbo;
	SSS::GL::Basic::IBO billboard_ibo;
	SSS::GL::Basic::VBO particles_vbo;
};



static bool sort_box(Particle& a, Particle& b) {

	return a._pos.z < b._pos.z;
}