#pragma once

#include "commons.h"
#include "Text_data.h"

auto constexpr epsilon = .2f;

auto constexpr BACKGROUND_LAYER		= 0.f;
auto constexpr ID_TEXT_LAYER		= BACKGROUND_LAYER + 4.f*epsilon;
auto constexpr MAIN_TEXT_LAYER		= BACKGROUND_LAYER + 3.f*epsilon;
auto constexpr COMMENT_TEXT_LAYER	= BACKGROUND_LAYER + 2.f*epsilon;
auto constexpr TAGS_TEXT_LAYER		= BACKGROUND_LAYER + 1.f*epsilon;


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
	glm::vec2 center2D();
	glm::vec3 centerZ0();
	//Check for a collision box/point
	bool check_collision(Particle p);
	//Check for a collision with another particle
	bool check_collision(glm::vec3 const& c_pos);
};

class BoxPlane : public SSS::GL::PlaneTemplate<BoxPlane> {

protected:
	virtual glm::mat4 _getTranslationMat4() const override;

private:
	glm::vec3 _offset;
public:
	inline glm::vec3 getOffset() const noexcept { return _offset; };
	void setOffset(glm::vec3 offset);
};

struct Tags : public Particle {
	Tags();
	Tags(std::string _name, std::string hex = "#FFFFFF", uint32_t weight = 1);
	~Tags();
	std::string _name;
	BoxPlane::Vector _model;
	uint32_t _weight;
};


class Box : public Particle {
public:
	Box() = default;
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
private:
	void _create_part(std::string s, const GUI_Layout& lyt, float layer, int flag = 0);

public:
	//Update the positions of all the subboxes 
	SSS::GL::Texture::Shared check_text_selection(glm::vec3 const& c_pos);

	//transformation
	//translation
	//rotation
	//... etc


	// DATA
	Text_data _td;   

	std::string _id;
	BoxPlane::Vector model;

	std::vector<uint16_t> tags;
	std::set<std::string> link_to;
	std::set<std::string> link_from;
	
	static std::map<uint16_t, Tags> tags_list;
	static std::map<std::string, GUI_Layout> layout_map;

	// DEFAULT VALUES
	static glm::vec2 minsize;
	glm::vec2 curs_pos = glm::vec2(0,0);

};

class Selection_square : Particle {
public:

	//Box rendering
	Particle model;

	static std::vector<Particle> box_batch;
};



static bool sortPlanes(std::shared_ptr<SSS::GL::PlaneBase>& a, std::shared_ptr<SSS::GL::PlaneBase>& b) {

	return a->getTranslation().z < b->getTranslation().z;
}