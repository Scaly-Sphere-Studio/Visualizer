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
	Node_Block(SceneGraph* p_Sg);
	std::string name() const { return "Block"; };

	glm::vec3 _pos;		// Node position and translations
	glm::vec3 _size;	// Node bounding box
	int _type = 1;

	float rotation = 0.f;

	//transforms
	virtual void _subjectUpdate(SSS::Subject const& subject, int event_id) override;
	virtual void update();
	glm::mat4 getLocalTransform(); // Rotation*Translation*Scale
	glm::mat4 getGlobalTransform();

private:
};

class Node_UI : public Node_Block
{
public:
	Node_UI(SceneGraph* p_Sg) :Node_Block(p_Sg) {};
	std::string name() const { return "UI"; };
	int _type = 2;

	//transforms
	void setVerticalOffset(const int& keyVO);		// Set the key Node to vertical offset
	void setHorizontalOffset(const int& keyHO);		// Set the key Node to horizontal offset
	void setDepthOffset(const int& keyDO);			// Set the key Node to depth offset
	virtual void setWrappingMin(const int& min) {};

	virtual void update();

	bool hidden = false;
	bool movable = false;
	//bool hidden = false;

	//STATES 
	bool isHovered() const noexcept;
	bool isClicked() const noexcept;
	bool isHeld() const noexcept;

	glm::vec3 center() { return _pos + _size * 0.5f; };
	bool _checkCollision(glm::vec2 const& r1p, glm::vec2 const& r1s, glm::vec2 const& r2p, glm::vec2 const& r2s)
	{
		if (r1p.y + r1s.y >= r2p.y &&      // r1 top edge past r2 bottom
			r1p.y <= r2p.y + r2s.y &&      // r1 bottom edge past r2 top
			r1p.x + r1s.x >= r2p.x &&      // r1 right edge past r2 left
			r1p.x <= r2p.x + r2s.x)        // r1 left edge past r2 right
		{
			LOG_MSG("Collision !!");
			return true;
		}
		return false;
	}

protected:
	int _hOffset = -1;	// key node for horizontal offset
	int _vOffset = -1;	// key node for vertical offset
	int _dOffset = -1;	// key node for depth offset

	virtual void translateElem() {};

};

class Node_Text : public Node_UI
{
public:
	Node_Text(SceneGraph* p_Sg, const std::string& s, const SSS::GUI_Layout& lyt = SSS::GUI_Layout{});
	std::string name() const { return "Text"; };

	TextPlane::Shared model;
	int _type = 4;

	virtual void _subjectUpdate(SSS::Subject const& subject, int event_id) override;
	virtual void setWrappingMin(const int& min);

	void setMaxStrSize(const int maxSize);
	void setTextColor(const SSS::RGBA_f& col);
	void setBackgroundColor(const SSS::RGBA_f& bgCol);
	void rotate(const float &rot) { model->rotate(glm::vec3(0, 0,rot)); };
	virtual void update();
private:
	void translateElem() { model->translate(_pos); };
	int _maxStrSize = 600;
	void _size_update();
};
