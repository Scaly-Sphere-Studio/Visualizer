#pragma once

#include "commons.h"
#include "Text_data.h"
#include "SSS/Commons/color.hpp"


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