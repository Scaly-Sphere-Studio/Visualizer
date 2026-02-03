#include "Node_Character.h"


void Node_Character::_register()
{
}


Node_Character::Node_Character(SSS::SceneGraph* p_Sg, const std::string& path)
{
	auto tex = SSS::GL::Texture::create(path);
	charAtlas = SSS::GL::Plane::create(tex);
	p_Sg->_rd->addPlane(charAtlas);


	translate(glm::vec3(-500, 0, 0));
	rotate(glm::vec3(0, 180, 0));

	
}


void Node_Character::_subjectUpdate(SSS::Subject const& subject, int event_id)
{
}

void Node_Character::update()
{
}

