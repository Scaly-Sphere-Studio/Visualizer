#include "Node_Box.h"

Node_Box::Node_Box(SceneGraph* p_Sg):
	Node_UI(p_Sg)
{
	_td.text_ID = SSS::toString(_key);
	_td.text = "Text";

	_pos = glm::vec3(150, 350, 0);
	_color = rand_pastel_color();

	Node_Text* first = new Node_Text(p_Sg, _td.text_ID);
	first->set_parent(this->_key);
	this->_children.emplace("ID", first->_key);
	_observe(*first);

	Node_Text* textNode = new Node_Text(p_Sg, _td.text);
	textNode->set_parent(this->_key);
	this->_children.emplace("TEXT", textNode->_key);
	_observe(*textNode);

	textNode->setVerticalOffset(first->_key);

	boxColor(_color);
	_resize();
	_sg->push(this);
}

void Node_Box::_subjectUpdate(SSS::Subject const& subject, int event_id)
{
	switch(event_id)
	{
	case SSS::EventList::Resize :
		_resize();
		return;
	}

}

void Node_Box::update()
{
	for (const auto &child : _children) 
	{
		Node_Text* t = (Node_Text*)_sg->at(child.second);
		t->update();
	}
}

void Node_Box::boxColor(const SSS::RGBA_f& col)
{
	glm::vec4 tex_col = SSS::RGBA_f(_color).to_HSL();
	glm::vec4 bg_col = tex_col;

	Node_Text* _id = (Node_Text*)_sg->at(_children["ID"]);
	tex_col.b = 0.3f;
	bg_col.b -= 0.15f;
	_id->setTextColor(SSS::RGBA_f::from_HSL(tex_col));
	_id->setBackgroundColor(SSS::RGBA_f::from_HSL((bg_col)));

	Node_Text* _txt = (Node_Text*)_sg->at(_children["TEXT"]);
	_txt->setBackgroundColor(SSS::RGBA_f{ _color });
}

void Node_Box::setColor(std::string hex)
{
	auto col = SSS::RGB24(hex);
	_color =glm::vec4(col.r, col.g, col.b, 255.f)/255.f;
}

void Node_Box::_resize()
{
	float min = 0;
	for (const auto& c : _children) {
		Node_Text* t = reinterpret_cast<Node_Text*>(_sg->at(c.second));
		min = std::max(min, t->_size.x);
	}
	
	Node_Text* last = reinterpret_cast<Node_Text*>(_sg->at(_children["TEXT"]));
	_size.x = min;
	_size.y = last->_pos.y - last->_size.y;


	for (const auto& c : _children) {
		Node_Text* t = reinterpret_cast<Node_Text*>(_sg->at(c.second));
		t->setWrappingMin(min);
	}
}
