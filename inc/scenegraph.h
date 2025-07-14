#pragma once

#include "commons.h"
#include "gui.h"

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

class Node 
{
public:
	Node() = default;
	Node(SceneGraph* p_Sg);
	~Node();

	int _key;
	int _type = 0;

	std::set<int> _parents;
	std::set<int> _children;

	//Template node
	Node* push(Node* n);

	void pop_child(const int& keyNode);
	void detach_parent(const int& keyNode);
	void add_parent(const int& keyNode);

	//to_string
	std::string to_string() const;
	operator std::string() const;

	virtual std::string name() const  { return "Node"; };

	SceneGraph* _sg;
	SSS::GL::PlaneRenderer::Weak _rd;
};



class Node_Block : public Node
{
public:
	Node_Block(SceneGraph* p_Sg) :Node(p_Sg) {};
	std::string name() const { return "Block"; };

	glm::vec3 _pos;
	int _type = 1;
};


class TextPlane : public SSS::GL::PlaneTemplate<TextPlane> {
	friend class SharedClass;
protected:
	virtual glm::mat4 _getTranslationMat4() const override;
private:
	TextPlane() = default;
	glm::vec3 _offset = glm::vec3{ 0 };
	Node* _parent;
public:
	inline void setParent(Node* node) { _parent = node; };
	inline auto getParent() const noexcept { return _parent; };
	inline glm::vec3 getOffset() const noexcept { return _offset; };
	void setOffset(glm::vec3 offset);
};


class Node_Text : public SSS::Observer, public Node
{
public:
	Node_Text(SceneGraph* p_Sg, const std::string &s, const SSS::GUI_Layout& lyt );
	std::string name() const { return "Text"; };

	glm::vec3 _pos;
	TextPlane::Shared model;
	int _type = 2;
};



class SceneGraph
{
public:
	SceneGraph() {};
	~SceneGraph() {};

	//Add the node to the nodelist and add it to the arborescence, to be used on free nodes
	void push(Node* n);	
	void emplace(Node* n);	// Add the node to the nodelist



	Node* at(const int & keyNode);

	//to_string
	std::string to_string() const;
	operator std::string() const;
	
	Node* operator[](const int &keyNode);
	std::unordered_map<int, Node*> _nodeList;
	std::vector<int> list;
private:

	SSS::GL::PlaneRenderer::Shared _rd;
	//std::unordered_map<std::string, SSS::GL::PlaneRenderer::Shared> _rdList;

};

