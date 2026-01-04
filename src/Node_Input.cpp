#include "Node_Input.h"

#include "scenegraph.h"

void Node_MouseInput::build()
{
	_radius = 200;

	glm::vec2 begin = glm::vec2(500, 450);
	glm::vec2 end = glm::vec2(500, 650);
	glm::vec4 color = glm::vec4(0.87, 0.53, 0.56, 1.0);
	glm::vec4 emptyColor = glm::vec4(0.118, 0.118, 0.118, 1.0);
	glm::vec4 background = glm::vec4(0.27f, 0.27f, 0.27f, 1.0f);

	UIPrimitive mouse;
	// Background
	mouse.shapeId = sdSegment;
	mouse.pos = begin;
	mouse.pos2 = end;
	mouse.size.r = _radius;
	mouse.color = glm::vec4(0.0);
	mouse.border = glm::vec4(1.0);
	mouse.borderWidth = 5.0;
	mouse.blendMode = GROUP;
	prims.push_back(mouse);

	mouse.shapeId = sdOrientedBox;
	mouse.pos = begin + glm::vec2(-85, -110);
	mouse.pos2 = mouse.pos + glm::vec2(0, 185);
	mouse.size.r = _radius * 2;
	mouse.blendMode = GROUP|SUBTRACT;
	prims.push_back(mouse);


	// Center button
	mouse.shapeId = sdSegment;
	mouse.pos = begin + glm::vec2(0, -50);
	mouse.pos2 = mouse.pos + glm::vec2(0, 80);
	mouse.size.r = _radius * 0.15;
	mouse.blendMode = 0;
	prims.push_back(mouse);


	// left click
	mouse.shapeId = sdSegment;
	mouse.pos = begin;
	mouse.pos2 = end;
	mouse.size.r = _radius;
	mouse.color = glm::vec4(0.0);
	mouse.border = glm::vec4(1.0);
	mouse.borderWidth = 5.0;
	mouse.blendMode = GROUP;
	prims.push_back(mouse);

	mouse.shapeId = sdSegment;
	mouse.pos = begin + glm::vec2(0, -50);
	mouse.pos2 = mouse.pos + glm::vec2(0, 80);
	mouse.size.r = _radius * 0.2;
	mouse.blendMode = SUBTRACT;
	prims.push_back(mouse);

	mouse.shapeId = sdOrientedBox;
	mouse.pos = begin + glm::vec2(-83, -100);
	mouse.pos2 = mouse.pos + glm::vec2(0, 170);
	mouse.size.r = _radius *.8 ;
	mouse.blendMode = INTERSECT | GROUP;
	prims.push_back(mouse);


	// Right click
	mouse.shapeId = sdSegment;
	mouse.pos = begin;
	mouse.pos2 = end;
	mouse.size.r = _radius;
	mouse.color = glm::vec4(0.0);
	mouse.border = glm::vec4(1.0);
	mouse.borderWidth = 5.0;
	mouse.blendMode = GROUP;
	prims.push_back(mouse);

	mouse.shapeId = sdSegment;
	mouse.pos = begin + glm::vec2(0, -50);
	mouse.pos2 = mouse.pos + glm::vec2(0, 80);
	mouse.size.r = _radius * 0.2;
	mouse.blendMode = SUBTRACT;
	prims.push_back(mouse);


	mouse.shapeId = sdOrientedBox;
	mouse.pos = begin + glm::vec2(-85 + _radius*.8 + 8, -100);
	mouse.pos2 = mouse.pos + glm::vec2(0, 170);
	mouse.size.r = _radius * .8;
	mouse.blendMode = INTERSECT | GROUP;
	prims.push_back(mouse);


	// Side buttons
	mouse.shapeId = sdRoundedBox;
	mouse.pos = begin + glm::vec2(-112, 60);
	mouse.pos2 = glm::vec2(0, 0);
	mouse.pos3 = glm::vec2(5, 0);
	mouse.size = glm::vec2(_radius*.03, 1.5*_radius*.1) ;
	mouse.blendMode = DEFAULT;
	prims.push_back(mouse);

	mouse.shapeId = sdRoundedBox;
	mouse.pos = mouse.pos + glm::vec2(0, 2*mouse.size.y + 5 );
	mouse.pos2 = glm::vec2(5, 0);
	mouse.pos3 = glm::vec2(0, 0);
	mouse.size = glm::vec2(_radius * .03, 1.5 * _radius * .1);
	mouse.blendMode = DEFAULT;
	prims.push_back(mouse);


	// trackball
	mouse.shapeId = sdCircle;
	mouse.pos = begin + glm::vec2(0, 200);
	mouse.pos2 = mouse.pos + glm::vec2(0, 80);
	mouse.size.r = _radius * 0.2;
	mouse.borderWidth = 5.0;
	mouse.border = glm::vec4(0.5);

	mouse.blendMode = 0;
	prims.push_back(mouse);

	mouse.shapeId = sdCircle;
	mouse.size.r = _radius * 0.1;
	mouse.color = glm::vec4(1.0);
	mouse.borderWidth = 0.0;
	mouse.color.a = 0.7;
	prims.push_back(mouse);

	origin = mouse.pos;


	_size = glm::vec3(end.x - begin.x + _radius, _radius, 0);
	_pos = glm::vec3(begin.x - _radius / 2, begin.y - _radius / 2, 0.f);


}



void Node_MouseInput::_subjectUpdate(SSS::Subject const& subject, int event_id) {
	if (_hidden)
		return;

	if (event_id == EVENT_ID("SSS_WINDOW_MOUSE_POSITION")) {
		const SSS::GL::Window& window = static_cast<SSS::GL::Window const&>(subject);
		int c_y, c_x;
		window.getCursorPos(c_x, c_y);

		glm::vec2 delta = glm::vec2(c_x, c_y) - old_cpos;
		float r = std::clamp(glm::sqrt(delta.x * delta.x + delta.y * delta.y), 0.f, 40.f);
		float theta = glm::atan2(delta.y, delta.x);

		prims[12].pos = origin + glm::vec2(r * glm::cos(theta), r * glm::sin(theta));
		//point.set_data(theta, r);

		old_cpos = glm::vec2(c_x, c_y);
	}

	if (event_id == EVENT_ID("SSS_WINDOW_MOUSE_INPUT")) {
		const SSS::GL::Window& window = static_cast<SSS::GL::Window const&>(subject);
		auto const& clicks = window.getClickInputs();

		// indexes
		// 2 center
		// 5 left click
		// 8 right click
		// 9 button up
		// 10 button down 
		auto showIpt = [&](int ipt, int index) {
			if (clicks[ipt].is_pressed()) {
				prims[index].color = glm::vec4(1.0);
			}

			if (clicks[ipt].is_released()) {
				prims[index].color = glm::vec4(0.0);
			}

			return;
		};

		int c_y, c_x;
		window.getCursorPos(c_x, c_y);

		showIpt(GLFW_MOUSE_BUTTON_1, 5);
		showIpt(GLFW_MOUSE_BUTTON_2, 8);
		showIpt(GLFW_MOUSE_BUTTON_3, 2);
		showIpt(GLFW_MOUSE_BUTTON_4, 10);
		showIpt(GLFW_MOUSE_BUTTON_5, 9);
	}
}
