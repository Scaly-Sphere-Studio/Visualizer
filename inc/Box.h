#pragma once

#include "commons.h"
#include "Text_data.h"

struct Particle {
	Particle();
	Particle(glm::vec3 _pos, glm::vec2 s, glm::vec4 _col);
	// ---------- Below data is passed to OpenGL VBO
	glm::vec3 _pos;
	glm::vec2 _size;
	glm::vec4 _color;
	GLuint _glsl_tex_unit{ UINT32_MAX };
	// ---------- Below data is purely internal and not passed to OpenGL
	uint32_t _sss_tex_id{ UINT32_MAX };
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

	void set_selected_col(std::string hex);
	void set_col(std::string hex);

	//STATES 
	bool _render = true;
	bool _hovered = false;
	bool _clicked = false;
	glm::vec3 selected_color = glm::vec3(0.93f, 0.64f, 0.43f);

	//Box rendering
	//Check for a collision box/point
	bool check_collision(glm::vec3 const& c_pos);
	//Return the coordinates of the center of the box
	glm::vec3 center();
	//Initialisation of the box and fill the model array
	void create_box();
	//Update the positions of all the subboxes 
	void update();

	// DATA
	Text_data _td;   

	std::string _id;
	std::vector<Particle> model;
	std::vector<uint16_t> tags;
	std::set<std::string> link_to;
	std::set<std::string> link_from;
	
	static std::vector<Particle> box_batch;
	static std::map<uint16_t, Tags> tags_list;
};

class BoxRenderer : public SSS::GL::Renderer {
	friend SSS::GL::Window;

private:
	BoxRenderer(std::weak_ptr<SSS::GL::Window> win, uint32_t id);

public:
	uint32_t cam_id{ 0 };

	virtual void render();

private:
	struct Batch {
		GLuint offset{ 0 };
		GLuint count{ 0 };
		std::vector<uint32_t> tex_ids;
	};

	SSS::GL::Basic::VAO::Ptr vao;
	SSS::GL::Basic::VBO::Ptr billboard_vbo;
	SSS::GL::Basic::IBO::Ptr billboard_ibo;
	SSS::GL::Basic::VBO::Ptr particles_vbo;
};

class Selection_square : Particle {
public:

	//Box rendering
	Particle model;

	static std::vector<Particle> box_batch;
};


static bool sort_box(Particle& a, Particle& b) {

	return a._pos.z < b._pos.z;
}