#include "scenegraph.h"

#include "Node_Primitive.h"
#include <random>

Node::Node()
{
	// Seed with a real random value, if available
	std::random_device r;
	std::default_random_engine e1(r());


}

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
		//_sg->pop(cKey);
	}
	_children.clear();
}


Node* Node::push(Node* n)
{
	return nullptr;
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
	//_parents.emplace(keyNode);
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
			res += "\t" + _sg->at(node)->to_string() + "\n";
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
	_cam = SSS::GL::Camera::create();
	_cam->setPosition({ 0, 0, 20.f });
	_cam->setZFar(40.f);
	_cam->setProjectionType(SSS::GL::Camera::Projection::OrthoFixed);

	_rd = SSS::GL::PlaneRenderer::create();
	_rd->camera = _cam;

}

void SceneGraph::push(Node* n)
{
	emplace(n);
	list.push_back(n->_key);
}

void SceneGraph::emplace(Node* n)
{
	_nodeList.emplace(n->_key, n);
}

int SceneGraph::Text(const std::string& s, const SSS::GUI_Layout& lyt)
{
	Node_Text* node = new Node_Text{ this, s, lyt };
	return node->_key;
}

int SceneGraph::Block(const glm::vec3& pos)
{
	Node_Block* node = new Node_Block{this};
	return node->_key;
}


Node* SceneGraph::at(const int& keyNode)
{
	if (_nodeList.contains(keyNode))
		return _nodeList.at(keyNode);

	return nullptr;
}

void SceneGraph::setCamera(SSS::GL::Camera::Shared pCam)
{
	_cam = pCam;
	_rd->camera = _cam;
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

Node* SceneGraph::operator[](const int& keyNode)
{
	if (_nodeList.contains(keyNode))
	{
		return _nodeList.at(keyNode);
	}
	return nullptr;
}