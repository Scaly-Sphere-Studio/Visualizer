#pragma once

#include "commons.h"
#include "Text_data.h"

auto constexpr BOX_LAYER = 2.f;

struct GUI_Layout {
	int32_t _ID = 0;

	SSS::TR::Format _fmt;
	int _marginh = 0, _marginv = 0;
};

class Box;

class BoxPlane : public SSS::GL::PlaneTemplate<BoxPlane> {
	friend class SharedClass;
protected:
	virtual glm::mat4 _getTranslationMat4() const override;
private:
	BoxPlane() = default;
	glm::vec3 _offset;
	SSS::SharedClass<Box>::Weak _parent;
public:
	inline void setBox(SSS::SharedClass<Box>::Weak box) { _parent = box; };
	inline auto getBox() const noexcept { return _parent.lock(); };
	inline glm::vec3 getOffset() const noexcept { return _offset; };
	void setOffset(glm::vec3 offset);
};

struct Tags {
	Tags();
	Tags(std::string _name, std::string hex = "#FFFFFF", uint32_t weight = 1);
	~Tags();
	std::string _name;
	BoxPlane::Vector _model;
	uint32_t _weight;
};


class Box : public SSS::Observer, public SSS::SharedClass<Box> {
	friend class SharedClass;

private:
	Box() = default;
public:
	~Box();

private:
	glm::vec2 _size;
	glm::vec3 _pos;
	glm::vec4 _color;

public:
	inline glm::vec2 getSize() const noexcept { return _size; };
	inline glm::vec3 getPos() const noexcept { return _pos; };
	inline glm::vec4 getColor() const noexcept { return _color; };

	bool checkCollision(glm::vec2 const& r2p, glm::vec2 const& r2s);
	bool checkCollision(std::shared_ptr<SSS::GL::PlaneBase> plane);

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
	
	virtual void _subjectUpdate(SSS::Subject const& subjet, int event_id) override;
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


static bool sortPlanes(std::shared_ptr<SSS::GL::PlaneBase>& a, std::shared_ptr<SSS::GL::PlaneBase>& b) {

	return a->getTranslation().z < b->getTranslation().z;
}