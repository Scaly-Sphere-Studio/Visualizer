#pragma once

#include "scenegraph.h"
#include "Node_Primitive.h"

#include "Text_data.h"
#include "SSS/Commons/color.hpp"



class Node_Box : public Node_Block 
{
public:
	Node_Box(SceneGraph* p_Sg);
	void setTextData(const Text_data& td) {};
	virtual void _subjectUpdate(SSS::Subject const& subject, int event_id) override;



private :
	glm::vec2 _size;
	glm::vec3 _pos;
	glm::vec4 _color;

	Text_data _td = Text_data{};

	std::vector<uint16_t> tags;
	std::set<std::string> link_to;
	std::set<std::string> link_from;
};
