#pragma once

#include "scenegraph.h"
#include "Node_Primitive.h"

#include "Text_data.h"
#include "SSS/Commons/color.hpp"
#include "SSS/Commons/eventList.hpp"
#include "Node_Export.h"



class Node_Box : public Node_UI 
{
public:
	Node_Box() = default;
	Node_Box(SceneGraph* p_Sg);
	Node_Box(SceneGraph* p_Sg, const Text_data& td);
	void setTextData(const Text_data& td);
	virtual void _subjectUpdate(SSS::Subject const& subject, int event_id) override;

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

private :
	int minWidth = 600;
	void _resize();
	Text_data _td = Text_data{};
};
