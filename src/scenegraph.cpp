#include "scenegraph.h"


//Node::Node(Node n)
//{
//	*this = n;
//}

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


void SceneGraph::push(Node* n)
{
	emplace(n);
	list.push_back(n->_key);
}

void SceneGraph::emplace(Node* n)
{
	_nodeList.emplace(n->_key, n);
}


Node* SceneGraph::at(const int& keyNode)
{
	if (_nodeList.contains(keyNode))
		return _nodeList.at(keyNode);

	return nullptr;
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

glm::mat4 TextPlane::_getTranslationMat4() const {
	glm::vec3 offset = _offset;
	auto texture = getTexture();
	if (texture) {
		auto const [w, h] = getTexture()->getCurrentDimensions();
		float const x = static_cast<float>(w) / 2.f;
		float const y = static_cast<float>(-h) / 2.f;
		offset += glm::vec3(x, y, 0);
	}
	return glm::translate(ModelBase::_getTranslationMat4(), offset);
}

Node_Text::Node_Text(SceneGraph* p_Sg, const std::string& s, const SSS::GUI_Layout& lyt)
	:Node(p_Sg)
{
	SSS::TR::Format fmt = lyt._fmt;
	auto area = SSS::TR::Area::create();
	auto plane = TextPlane::create(SSS::GL::Texture::create(area));
	//plane->setBox(nullptr);
	_observe(*plane->getTexture());


	glm::vec4 tex_col = SSS::RGBA_f(BLACK).to_HSL();
	glm::vec4 bg_col = tex_col;

	tex_col.b = 0.3f;
	fmt.text_color = SSS::RGBA_f::from_HSL(tex_col);

	bg_col.b -= 0.15f;
	//area->setClearColor(SSS::RGBA_f::from_HSL((bg_col)));
	//area->setClearColor(static_cast<SSS::RGBA32>(SSS::RGBA_f{ BLACK }));
	area->setFocusable(true);
	area->setWrapping(true);
	area->setMargins(lyt._marginv, lyt._marginh);
	area->setWrappingMaxWidth(TEXT_MAX_WIDTH);
	area->setFormat(fmt);
	area->parseString(s);

	//Create the model
	plane->translate(_pos);

	plane->setHitbox(SSS::GL::Plane::Hitbox::Full);
	model = plane;
}
