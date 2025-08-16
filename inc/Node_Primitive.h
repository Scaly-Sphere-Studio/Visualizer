#pragma once

#include "scenegraph.h"

#include "commons.h"
#include "gui.h"

#include "Text_data.h"
#include "SSS/Commons/color.hpp"


static std::map<std::string, SSS::GUI_Layout> layout_map;


class SceneGraph;

// Utility for Text Nodes

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

// 3D Node for grouping elements
class Node_Block : public Node
{
public:
	Node_Block(SceneGraph* p_Sg) :Node(p_Sg) {};
	std::string name() const { return "Block"; };

	glm::vec3 _pos = glm::vec3{ 0, 0, 0 };	// Node position and translations
	glm::vec3 _size = glm::vec3{ 0, 0, 0 };	// Node bounding box
	int _type = 1;


	//transforms
	virtual void _subjectUpdate(SSS::Subject const& subject, int event_id) override;
private:
};

class Node_UI : public Node_Block
{
public:
	Node_UI(SceneGraph* p_Sg) :Node_Block(p_Sg) {};
	std::string name() const { return "UI"; };
	int _type = 2;

	//transforms
	void setVerticalOffset(Node_UI* pVO)	{ _vOffset = pVO; }; // Set the ptr Node to vertical offset
	void setHorizontalOffset(Node_UI* pHO)	{ _hOffset = pHO; }; // Set the ptr Node to horizontal offset
	void setDepthOffset(Node_UI* pDO)		{ _dOffset = pDO; }; // Set the ptr Node to depth offset

private:
	Node_UI* _hOffset = nullptr;	// horizontal offset
	Node_UI* _vOffset = nullptr;	// vertical offset
	Node_UI* _dOffset = nullptr;	// depth offset
};

class Node_Text : public Node_Block
{
public:
	Node_Text(SceneGraph* p_Sg, const std::string& s, const SSS::GUI_Layout& lyt = SSS::GUI_Layout{});
	std::string name() const { return "Text"; };

	TextPlane::Shared model;
	int _type = 4;

	virtual void _subjectUpdate(SSS::Subject const& subject, int event_id) override;
	void setMaxStrSize(const int maxSize);
private:
	int _maxStrSize = 600;
	void _size_update();
};
