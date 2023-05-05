#pragma once

#include "commons.h"
#include "Text_data.h"

#define epsilon 1.0f / static_cast<float>(RAND_MAX * 100.0f)

#define BACKGROUND_LAYER		0
#define BACKGROUND_COLOR_LAYER	epsilon
#define INFO_TEX_LAYER			2*epsilon
#define UNDER_TXT_LAYER			3*epsilon
#define TXT_LAYER				4*epsilon


struct GUI_Layout {
	int32_t _ID = 0;

	SSS::TR::Format _fmt;
	int _marginh = 0, _marginv = 0;
};

struct Particle {
	Particle();
	Particle(glm::vec3 pos, glm::vec2 s, glm::vec4 col);
	// ---------- Below data is passed to OpenGL VBO
	glm::vec3 _pos;
	glm::vec2 _size;
	glm::vec4 _color;
	GLuint _glsl_tex_unit{ UINT32_MAX };
	// ---------- Transformations
	// float angle;
	glm::vec3 translation;
	// float scale;

	// ---------- Below data is purely internal and not passed to OpenGL
	SSS::GL::Texture::Shared _sss_texture;
	
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
	std::string _name;
	std::vector<Particle> _model;
	uint32_t _weight;
};


class Box : public Particle{
public:
	Box();
	Box(glm::vec3 _pos, glm::vec2 _s, std::string hex = "000000");
	~Box();
	
	//Update the position with a delta for dragging the boxes
	void update();

	void set_selected_col(std::string hex);
	void set_col(std::string hex);
	void set_text_data(const Text_data& td);

	//STATES 
	bool _render = true;
	bool _hovered = false;
	bool _clicked = false;

	bool _show_comment = false;
	bool _show_tags = false;
	glm::vec3 selected_color = glm::vec3(0.93f, 0.64f, 0.43f);

	//Box rendering
	
	//Initialisation of the box and fill the model array
	void create_box();
	//Update the positions of all the subboxes 
	SSS::GL::Texture::Shared check_text_selection(glm::vec3 const& c_pos);

	//transformation
	//translation
	//rotation
	//... etc


	// DATA
	Text_data _td;   

	std::string _id;
	std::vector<Particle> model;

	std::vector<uint16_t> tags;
	std::set<std::string> link_to;
	std::set<std::string> link_from;
	
	static std::vector<Particle> box_batch;
	static std::map<uint16_t, Tags> tags_list;
	static std::map<std::string, GUI_Layout> layout_map;

	// DEFAULT VALUES
	static glm::vec2 minsize;
	glm::vec2 curs_pos = glm::vec2(0,0);

};

void text_frame(std::string s, const GUI_Layout& lyt, Box& b, int flag = 0);
void background_frame(Box& b);

class Selection_square : Particle {
public:

	//Box rendering
	Particle model;

	static std::vector<Particle> box_batch;
};


class BoxRenderer : public SSS::GL::Renderer<BoxRenderer> {
	friend SSS::GL::Basic::SharedBase<BoxRenderer>;

private:
	BoxRenderer();

public:
	SSS::GL::Camera::Shared camera;

	virtual void render();

private:
	struct Batch {
		GLuint offset{ 0 };
		GLuint count{ 0 };
		std::vector<SSS::GL::Texture::Shared> textures;
	};

	SSS::GL::Basic::VAO vao;
	SSS::GL::Basic::VBO billboard_vbo;
	SSS::GL::Basic::IBO billboard_ibo;
	SSS::GL::Basic::VBO particles_vbo;
};



static bool sort_box(Particle& a, Particle& b) {

	return a.translation.z < b.translation.z;
}