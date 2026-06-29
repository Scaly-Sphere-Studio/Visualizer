#pragma once

#include "commons.h"

#include <SSS/GL/Objects/Models/Shapes.hpp>
#include <SSS/Settings/Theme.h>

static bool sortPlanes(std::shared_ptr<SSS::GL::PlaneBase>& a, std::shared_ptr<SSS::GL::PlaneBase>& b) {

	return a->getTranslation().z < b->getTranslation().z;
}