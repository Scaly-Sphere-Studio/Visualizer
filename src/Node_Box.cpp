#include "Node_Box.h"



//std::once_flag _Node_Box_Registry;

void Node_Box::_register()
{
}


Node_Box::Node_Box():
	SSS::Node_UI()
{
	//std::call_once(_Node_Box_Registry, [&]() { _register(); });

	_td.text_ID = SSS::toString(_key);
	_td.text = "Text";

	_color = rand_pastel_color();

	SSS::Node_Text* first = new SSS::Node_Text(_td.text_ID);
	first->set_parent(this->_key);
	this->_children.emplace("ID", first->_key);
	_observe(*first);
	SSS::SceneGraph::getCurrentRenderer()->addPlane(first->model);

	SSS::Node_Text* textNode = new SSS::Node_Text(_td.text);
	textNode->set_parent(this->_key);
	this->_children.emplace("TEXT", textNode->_key);
	_observe(*textNode);
	SSS::SceneGraph::getCurrentRenderer()->addPlane(textNode->model);

	textNode->setVerticalOffset(first->_key);

	setColor(_color);
	_resize();
	SSS::SceneGraph::push(this);
}

Node_Box::Node_Box(const Text_data& td):
	Node_UI()
{
	_register();
	_td = td;

	_pos = glm::vec3(150, 350, 0);
	_color = rand_pastel_color();

	SSS::Node_Text* first = new SSS::Node_Text(_td.text_ID);
	this->_children.emplace("ID", first->_key);
	first->set_parent(this->_key);
	_observe(*first);
	SSS::SceneGraph::getCurrentRenderer()->addPlane(first->model);

	SSS::Node_Text* textNode = new SSS::Node_Text(_td.text);
	this->_children.emplace("TEXT", textNode->_key);
	textNode->set_parent(this->_key);
	_observe(*textNode);
	SSS::SceneGraph::getCurrentRenderer()->addPlane(textNode->model);

	textNode->setVerticalOffset(first->_key);

	setColor(_color);
	_resize();
	SSS::SceneGraph::push(this);
}

void Node_Box::setTextData(const Text_data& td)
{
	SSS::Node_Text* _id = (SSS::Node_Text*)SSS::SceneGraph::at(_children["ID"]);
	_id->parseText(td.text_ID);

	SSS::Node_Text* _txt = (SSS::Node_Text*)SSS::SceneGraph::at(_children["TEXT"]);
	_txt->parseText(td.text);

	update();

}

void Node_Box::_subjectUpdate(SSS::Subject const& subject, SSS::Event const& event)
{
	int const event_id = event.id;
	if (event_id == EVENT_ID("NODE_TEXT_RESIZE")) {
		_resize();
		return;
	}

}

void Node_Box::update()
{
	for (const auto &child : _children) 
	{
		SSS::Node_Text* t = (SSS::Node_Text*)SSS::SceneGraph::at(child.second);
		t->update();
	}
}

void Node_Box::setColor(const SSS::RGBA_f& col)
{
	_color = col;
	glm::vec4 tex_col = SSS::RGBA_f(col).to_HSL();
	glm::vec4 bg_col = tex_col;

	SSS::Node_Text* _id = (SSS::Node_Text*)SSS::SceneGraph::at(_children["ID"]);
	tex_col.b = 0.3f;
	bg_col.b -= 0.15f;
	_id->setTextColor(SSS::RGBA_f::from_HSL(tex_col));
	_id->setBackgroundColor(SSS::RGBA_f::from_HSL((bg_col)));

	SSS::Node_Text* _txt = (SSS::Node_Text*)SSS::SceneGraph::at(_children["TEXT"]);
	_txt->setBackgroundColor(col);
	tex_col.b = 0.15f;

}

bool Node_Box::checkCollision(std::shared_ptr<SSS::GL::PlaneBase> plane) const
{
	glm::vec2 const pos = plane->getTranslation();
	glm::vec2 const size = plane->getScaling();

	return checkCollision2D(pos - (size / 2.f), size);
}

void Node_Box::readExport(const Export_Node_Box& expNode)
{

	//_td.text_ID		= expNode.id;
	_pos			= expNode.pos;
	_color			= expNode.color;
	tags			= expNode.tags;
	link_to			= expNode.link_to;
	link_from		= expNode.link_from;

	setColor(_color);
	update();
}

Export_Node_Box Node_Box::export_node() const 
{
	Export_Node_Box ex;
	ex.id			= _td.text_ID;
	ex.pos			= _pos;
	ex.color		= _color;
	ex.tags			= tags;
	ex.link_to		=  link_to;
	ex.link_from	= link_from;
	return ex;
}

void Node_Box::_resize()
{
	float min = minWidth;
	for (const auto& c : _children) {
		SSS::Node_Text* t = reinterpret_cast<SSS::Node_Text*>(SSS::SceneGraph::at(c.second));
		min = std::max(min, t->_size.x);
	}
	
	SSS::Node_Text* last = reinterpret_cast<SSS::Node_Text*>(SSS::SceneGraph::at(_children["TEXT"]));
	_size.x = min;
	_size.y = last->_pos.y - last->_size.y;


	for (const auto& c : _children) {
		SSS::Node_Text* t = reinterpret_cast<SSS::Node_Text*>(SSS::SceneGraph::at(c.second));
		t->setWrappingMin(static_cast<unsigned int>(std::ceil(min)));
	}

	//EMIT_EVENT();
	//_notifyObservers(SSS::EventList::Resize);
}



//void to_json(nlohmann::json& j, const Node_Box& t)
//{
//	j = nlohmann::json{
//	{"ID"       , t._td.text_ID},
//	{"COLOR"    , t._color.to_Hex()},
//	{"POSITION" , t._pos} };
//
//	if (t.tags.size())j["TAGS"] = t.tags;
//	if (t.link_to.size())j["LINK_TO"] = t.link_to;
//	if (t.link_from.size())j["LINK_FROM"] = t.link_from;
//}
//
//void from_json(const nlohmann::json& j, Node_Box& t)
//{
//	j.at("ID").get_to(t._td.text_ID);
//	std::string hex;
//	j.at("COLOR").get_to(hex);
//	t._color = SSS::RGBA_f(hex);
//	t._pos = j.at("POSITION");
//
//	//optional fields
//	if (j.contains("TAGS") && !j["TAGS"].is_null()) {
//		j.at("TAGS").get_to(t.tags);
//	}
//	if (j.contains("LINK_TO") && !j["LINK_TO"].is_null()) {
//		j.at("LINK_TO").get_to(t.link_to);
//	}
//	if (j.contains("LINK_FROM") && !j["LINK_FROM"].is_null()) {
//		j.at("LINK_FROM").get_to(t.link_from);
//	}
//}
//
//
//void to_json(nlohmann::json& j, const Node_Box*& t)
//{
//	j = nlohmann::json{
//	{"ID"       , t->_td.text_ID},
//	{"COLOR"    , t->_color.to_Hex()},
//	{"POSITION" , t->_pos} };
//
//	if (t->tags.size())j["TAGS"]			= t->tags;
//	if (t->link_to.size())j["LINK_TO"]		= t->link_to;
//	if (t->link_from.size())j["LINK_FROM"]	= t->link_from;
//}
//
//void from_json(const nlohmann::json& j, Node_Box*& t)
//{
//	j.at("ID").get_to(t->_td.text_ID);
//	std::string hex;
//	j.at("COLOR").get_to(hex);
//	t->_color = SSS::RGBA_f(hex);
//	t->_pos = j.at("POSITION");
//
//	//optional fields
//	if (j.contains("TAGS") && !j["TAGS"].is_null()) {
//		j.at("TAGS").get_to(t->tags);
//	}
//	if (j.contains("LINK_TO") && !j["LINK_TO"].is_null()) {
//		j.at("LINK_TO").get_to(t->link_to);
//	}
//	if (j.contains("LINK_FROM") && !j["LINK_FROM"].is_null()) {
//		j.at("LINK_FROM").get_to(t->link_from);
//	}
//}

