 #pragma once

#include <SSS/Commons.hpp>
#include <SSS/SceneGraph/Node_UI.h>

#include "commons.h"
#include "Text_data.h"

#include "Node_Export.h"




class Node_Box : public SSS::Node_UI, public SSS::_EventRegistry<Node_Box>
{
public:
	friend _EventRegistry<Node_Box>;
	Node_Box() = default;
	Node_Box(const Node_Box&) = default;
	Node_Box(SSS::SceneGraph* p_Sg);
	Node_Box(SSS::SceneGraph* p_Sg, const Text_data& td);
	void setTextData(const Text_data& td);
	virtual void _subjectUpdate(SSS::Subject const& subject, SSS::Event const& event) override;

	void update();

	void setColor(const SSS::RGBA_f& col);
	bool checkCollision(std::shared_ptr<SSS::GL::PlaneBase> plane) const;
	void readExport(const Export_Node_Box& expNode);

	bool _show_comment	= false;
	bool _show_tags		= false;

	SSS::RGBA_f _color;
	std::vector<uint16_t> tags;
	std::set<std::string> link_to;
	std::set<std::string> link_from;

	Export_Node_Box export_node() const;
	Text_data getData() const { return _td; };


	Text_data _td = Text_data{};
private :
	static void _register();
	int minWidth = 600;
	void _resize();
};

//void to_json(nlohmann::json& j, const Node_Box& t);
//void from_json(const nlohmann::json& j, Node_Box& t);
//
//void to_json(nlohmann::json& j, const Node_Box*& t);
//void from_json(const nlohmann::json& j, Node_Box*& t);