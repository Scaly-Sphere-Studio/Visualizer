#pragma once

#include "Node_UI.h"

class  Node_MouseInput : public Node_UI
{
public:
	Node_MouseInput() = default;
	~Node_MouseInput() = default;
	std::string name() const { return "Mouse Input"; };
	int _type = 4;

	void build();

	virtual void _subjectUpdate(SSS::Subject const& subject, int event_id) override;

private:
	float _radius;
	void _size_update() {};

	glm::vec2 origin;
	glm::vec2 old_cpos;
};

