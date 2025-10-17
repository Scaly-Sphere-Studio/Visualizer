#pragma once

#include "commons.h"
#include "Text_data.h"
#include "SSS/Commons/color.hpp"

//std::map<uint16_t, Tags>Box::tags_list{};
//std::map<std::string, SSS::GUI_Layout> Box::layout_map{};
//glm::vec2 Box::minsize = glm::vec2{ 150,75 };

auto constexpr BOX_LAYER = 2.f;
namespace SSS
{
	struct GUI_Layout 
	{
		int32_t _ID = 0;

		SSS::TR::Format _fmt;
		int _marginh = 0, _marginv = 0;
	};
}

static bool sortPlanes(std::shared_ptr<SSS::GL::PlaneBase>& a, std::shared_ptr<SSS::GL::PlaneBase>& b) {

	return a->getTranslation().z < b->getTranslation().z;
}