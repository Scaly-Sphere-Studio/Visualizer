#pragma once

#include <SSS/Commons.hpp>
#include <SSS/SceneGraph/Node_UI.h>
enum class AnimationType {
	ROTATION,
	SCALE,
	TRANSLATION
};



class Node_Character : public SSS::Node_UI, public SSS::_EventRegistry<Node_Character>
{
public:
	friend _EventRegistry<Node_Character>;
	Node_Character(const std::string& path);
	virtual void _subjectUpdate(SSS::Subject const& subject, SSS::Event const& event) override;

	void update();

	void translate(const glm::vec3 t) { charAtlas->translate(t); };
	//void rotate(const glm::vec3 r) { charAtlas->rotate(r); };
	void rotate(const glm::vec3 r) { charAtlas->setRotation(r); };

	std::string dir_path;

	int frame = 0;
private :

	SSS::GL::Plane::Shared charAtlas;
	Node_Character() = default;
	static void _register();
};
