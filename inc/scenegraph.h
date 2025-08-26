#pragma once

#include "commons.h"
#include "gui.h"


class SceneGraph;

class Node : public SSS::Observer, public SSS::Subject
{
public:
	Node();
	Node(SceneGraph* p_Sg);
	~Node();

	int _key;
	int _type = 0;
	bool _inherited_transform = true;

	int _parent;
	std::set<int> _children;


	//Template node
	Node* push(Node* n);

	virtual void update() {};

	void pop_child(const int& keyNode);
	void detach_parent(const int& keyNode);
	void set_parent(const int& keyNode) { _parent = keyNode; };

	//to_string
	std::string to_string() const;
	operator std::string() const;

	virtual std::string name() const  { return "Node"; };

	SceneGraph* _sg;
	SSS::GL::PlaneRenderer::Weak _rd;
};


class SceneGraph
{
public:
	SceneGraph();
	~SceneGraph() {};

	//Add the node to the nodelist and add it to the arborescence, to be used on free nodes
	void push(Node* n);	
	void emplace(Node* n);	// Add the node to the nodelist

	int Text(const std::string& s, const SSS::GUI_Layout& lyt = SSS::GUI_Layout{});
	int Block(const glm::vec3 &pos = glm::vec3(0));


	Node* at(const int & keyNode);

	void setCamera(SSS::GL::Camera::Shared pCam);

	//to_string
	std::string to_string() const;
	operator std::string() const;
	
	Node* operator[](const int &keyNode);
	std::unordered_map<int, Node*> _nodeList;
	std::vector<int> list;

	SSS::GL::PlaneRenderer::Shared _rd;
private:

	SSS::GL::Camera::Shared _cam;

	//std::unordered_map<std::string, SSS::GL::PlaneRenderer::Shared> _rdList;

};

