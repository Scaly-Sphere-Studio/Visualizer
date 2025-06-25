#pragma once

#include "commons.h"


class SceneGraph;

class Node 
{
public:
	Node(SceneGraph* p_Sg);
	~Node();

	int _key;
	

	std::set<int> _parents;
	std::set<int> _children;
	void push(Node &n);
	void pop_child(const int& keyNode);
	void detach_parent(const int& keyNode);
	void add_parent(const int& keyNode);

	//to_string
	std::string to_string() const;
	operator std::string() const;

	std::string name() const  { return "Node"; };

	SceneGraph* _sg;
	SSS::GL::PlaneRenderer::Weak _rd;
};


class Node_Block : public Node
{

};


class SceneGraph
{
public:
	SceneGraph();
	~SceneGraph();

	//Add the node to the nodelist and add it to the arborescence, to be used on free nodes
	void push(const Node &n);	


	void emplace(const Node &n);	// Add the node to the nodelist
	void pop(const int &keyNode);	// Delete the node from the nodelist

	std::weak_ptr<Node> at(const int & keyNode);

	//to_string
	std::string to_string() const;
	operator std::string() const;
	//
	std::weak_ptr<Node> operator[](const int &keyNode);
private:
	std::unordered_map<int, std::shared_ptr<Node>> _nodeList;
	std::vector<int> list;

	SSS::GL::PlaneRenderer::Shared _rd;
	//std::unordered_map<std::string, SSS::GL::PlaneRenderer::Shared> _rdList;

};