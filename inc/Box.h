#pragma once

#include "commons.h"
#include "Text_data.h"

auto constexpr BOX_LAYER = 2.f;

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


class Box : public SSS::GL::Basic::SharedBase<Box> {
	friend class SharedBase;

private:
	Box() = default;
public:
	~Box();

	using SharedBase::Shared;
	using SharedBase::create;

private:
	glm::vec2 _size;
	glm::vec3 _pos;
	glm::vec4 _color;

public:
	inline glm::vec2 getSize() const noexcept { return _size; };
	inline glm::vec3 getPos() const noexcept { return _pos; };
	inline glm::vec4 getColor() const noexcept { return _color; };

	glm::vec3 center() const;

	//inline void setSize(glm::vec2 s) {};
	void setPos(glm::vec3 pos);
	inline void setZ(float z) { /*setPos(glm::vec3(_pos.x, _pos.y, z));*/ };
	void setColor(glm::vec4 color);
	inline void setColor(std::string hex) { setColor(hex_to_rgb(hex)); };

	//void setSelectedCol(std::string hex);

	void set_text_data(const Text_data& td);

	//STATES 
	bool isHovered() const noexcept;
	bool isClicked() const noexcept;
	bool isHeld() const noexcept;

	bool _show_comment = false;
	bool _show_tags = false;
	glm::vec3 selected_color = glm::vec3(0.93f, 0.64f, 0.43f);

	//Box rendering
	
	//Initialisation of the box and fill the model array
	void create_box();
private:
	void _create_part(std::string s, const GUI_Layout& lyt, int flag = 0);
	void _size_update();

public:

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