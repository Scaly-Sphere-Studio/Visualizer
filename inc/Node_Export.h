#pragma once

#include "scenegraph.h"
#include "Node_Primitive.h"
#include "Text_data.h"
#include <optional>


class Export_Node_Box
{
public:
	std::string id;
	glm::vec3 pos;
	SSS::RGBA_f color;
	std::vector<uint16_t> tags;
	std::set<std::string> link_to;
	std::set<std::string> link_from;
	//Text_data td;
};

void to_json(nlohmann::json& j, const Export_Node_Box& t);
void from_json(const nlohmann::json& j, Export_Node_Box& t);