#pragma once

#include "scenegraph.h"

#include "commons.h"
#include "gui.h"

#include "Text_data.h"
#include "SSS/Commons/color.hpp"

#define BLACK glm::vec4(0,0,0,1)
#define WHITE glm::vec4(1,1,1,1)

#define TEXT_MAX_WIDTH          600


namespace SSS
{
	//struct GUI_Layout {
	//	int32_t _ID = 0;

	//	SSS::TR::Format _fmt;
	//	int _marginh = 0, _marginv = 0;
	//	
	//};
}

static std::map<std::string, SSS::GUI_Layout> layout_map;


class SceneGraph;

class Node_Block : public Node
{
public:
	Node_Block(SceneGraph* p_Sg) :Node(p_Sg), _pos(glm::vec3(0)) {};
	std::string name() const { return "Block"; };

	glm::vec3 _pos;
	int _type = 1;

	virtual void _subjectUpdate(SSS::Subject const& subject, int event_id) override;

};


class TextPlane : public SSS::GL::PlaneTemplate<TextPlane> {
	friend class SharedClass;
protected:
	virtual glm::mat4 _getTranslationMat4() const override;
private:
	TextPlane() = default;
	glm::vec3 _offset = glm::vec3{ 0 };
	Node* _parent = nullptr;
public:
	inline void setParent(Node* node) { _parent = node; };
	inline auto getParent() const noexcept { return _parent; };
	inline glm::vec3 getOffset() const noexcept { return _offset; };
	void setOffset(glm::vec3 offset) {};
};


class Node_Text : public Node
{
public:
	Node_Text(SceneGraph* p_Sg, const std::string& s, const SSS::GUI_Layout& lyt);
	std::string name() const { return "Text"; };

	glm::vec3 _pos = glm::vec3{ -150, 25, 0 };
	glm::vec2 _size = glm::vec2{ 0, 0 };
	TextPlane::Shared model;
	int _type = 2;

	virtual void _subjectUpdate(SSS::Subject const& subject, int event_id) override;

	void _size_update();
};
