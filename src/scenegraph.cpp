#include "scenegraph.h"


Node::Node(SceneGraph* p_Sg)
{
	_key = 0;
	_sg = p_Sg;
}

Node::~Node()
{
	_parents.clear();

	for (int cKey : _children) 
	{
		_sg->pop(cKey);
	}
	_children.clear();
}

void Node::push(Node& n)
{
	n.add_parent(_key);
	_sg->emplace(n);
	_children.emplace(n._key);
}

void Node::pop_child(const int& keyNode)
{

}

void Node::detach_parent(const int& keyNode)
{
	_sg[keyNode];
}

void Node::add_parent(const int& keyNode)
{
	_parents.emplace(keyNode);
}

std::string Node::to_string() const
{
	std::string res;
	res = "Node : " + name() + "(" + SSS::toString(_key) + ")\n";

	//print parent list
	if (!_parents.empty()) {
		res += "\tparents :\n";
		for (auto node : _parents) {
			res += "\t id : " + std::to_string(node) + "\n";
		}
	}

	//print children description
	if (!_children.empty()) {
		res += "\tchildren :\n";
		for (auto node : _children) {
			res += "\t" + _sg->at(node).lock()->to_string() + "\n";
		}
	}

	return res;
}

Node::operator std::string() const
{
	return to_string();
}


SceneGraph::SceneGraph()
{

}

SceneGraph::~SceneGraph()
{
}

void SceneGraph::push(const Node& n)
{
	_nodeList.emplace(n._key, std::make_shared<Node>(n));
	list.push_back(n._key);
}

void SceneGraph::emplace(const Node& n)
{
	_nodeList.emplace(n._key, std::make_shared<Node>(n));
}


void SceneGraph::pop(const int& keyNode)
{
	auto it = std::find(list.begin(), list.end(), keyNode);
	if (it != list.end()) {
		list.erase(it);
	}
	_nodeList.erase(keyNode);
}

std::weak_ptr<Node> SceneGraph::at(const int& keyNode)
{
	if (_nodeList.contains(keyNode))
		return _nodeList.at(keyNode);

	return std::weak_ptr<Node>();
}

std::string SceneGraph::to_string() const
{
	std::string res;
	for (const int keyNode : list) {
		res += _nodeList.at(keyNode)->to_string() + "\n";
	}

	return res;
}

SceneGraph::operator std::string() const
{
	return to_string();
}

std::weak_ptr<Node> SceneGraph::operator[](const int& keyNode)
{
	if (_nodeList.contains(keyNode))
	{
		return _nodeList.at(keyNode);
	}
	return std::weak_ptr<Node>();
}
