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

//Flags
enum SDF_BlendModes {
	DEFAULT		= 0,
	GROUP		= 1,
	SUBTRACT	= 2,
	INTERSECT	= 4
};


enum SDF_Shapes {
	sdCircle,
	sdOrientedBox,
	sdRoundedBox,
	sdSegment,
	sdPie,
	sdRing,
	sdArc,
	sdTriangle,
	sdRounedX,
	sdCross,
	sdPentagon,
	sdHexagon,
};

// ! ordre important !
struct UIPrimitive {
	glm::vec2 pos		= glm::vec2{};						// 8 bytes
	glm::vec2 size		= glm::vec2{};						// 8 bytes

	glm::vec4 color		= glm::vec4{};					// 16 bytes
	glm::vec4 border	= glm::vec4{};					// 16 bytes

	float borderWidth	= 0.f;			// 4 bytes
	float cornerRadius	= 0.f;			// 4 bytes
	int   shapeId		= sdCircle;     // 4 bytes
	int   blendMode		= DEFAULT;      // 4 bytes

	float innerRadius	= 0.f;			// 4 bytes
	float progress		= 0.f;			// 4 bytes
	glm::vec2   pos2	= glm::vec2{};					// 8bytes

	glm::vec2   pos3	= glm::vec2{};					// 8 bytes
	glm::vec2   pos4	= glm::vec2{};					// 8 bytes

	float rotation		= 0.f;			// 4 bytes
	float scale			= 0.f;			// 4 bytes
	float _pad			= 0.f;			// 4 bytes
	float _pad1			= 0.f;			// 4 bytes
};

static bool sortPlanes(std::shared_ptr<SSS::GL::PlaneBase>& a, std::shared_ptr<SSS::GL::PlaneBase>& b) {

	return a->getTranslation().z < b->getTranslation().z;
}