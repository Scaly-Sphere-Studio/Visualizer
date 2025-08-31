#pragma once

#include "scenegraph.h"
#include "Node_Primitive.h"

#include "Text_data.h"
#include "SSS/Commons/color.hpp"
#include "SSS/Commons/eventList.hpp"



class Node_Box : public Node_UI 
{
public:
	Node_Box(SceneGraph* p_Sg);
	void setTextData(const Text_data& td) {};
	virtual void _subjectUpdate(SSS::Subject const& subject, int event_id) override;

	void update();

	void boxColor(const SSS::RGBA_f& col);

	bool _show_comment	= false;
	bool _show_tags		= false;

	glm::vec4 _color;
	void setColor(std::string hex);

private :

	void _resize();
	Text_data _td = Text_data{};

	std::vector<uint16_t> tags;
	std::set<std::string> link_to;
	std::set<std::string> link_from;
};
