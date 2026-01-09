#include "Node_UI.h"

#include "scenegraph.h"

#define BLACK glm::vec4(0,0,0,1)
#define WHITE glm::vec4(1,1,1,1)

void Node_UI::boundingBox()
{
	//BB
	UIPrimitive bb;
	bb.shapeId = sdSegment;
	bb.pos = _pos;
	bb.pos2 = _pos + glm::vec3(_size.x, 0.0, 0.0);
	bb.size.x = 3;
	bb.color = glm::vec4(0.0, 0.0, 0.0, 0.5);
	prims.push_back(bb);

	bb.shapeId = sdSegment;
	bb.pos = _pos + glm::vec3(_size.x, 0.0, 0.0);
	bb.pos2 = _pos + _size;
	prims.push_back(bb);

	bb.shapeId = sdSegment;
	bb.pos = _pos + _size;
	bb.pos2 = _pos + glm::vec3(0.0, _size.y, 0.0);
	prims.push_back(bb);

	bb.shapeId = sdSegment;
	bb.pos = _pos + glm::vec3(0.0, _size.y, 0.0);
	bb.pos2 = _pos;
	prims.push_back(bb);
}

void Node_UI::_register()
{
	REGISTER_EVENT("NODE_UI_HOVER");
	REGISTER_EVENT("NODE_UI_MOUSE_LEFT");
}

void Node_UI::setColor(const std::string& hex)
{
	setColor(SSS::RGBA_f(hex));
}

void Node_UI::setVerticalOffset(const int& keyVO)
{
	_vOffset = keyVO;
	Node_Block* UIelem = static_cast<Node_Block*>(_sg->at(_vOffset));
	_pos.y += UIelem->_pos.y;
	_pos.y -= UIelem->_size.y;

	translateElem();
}

void Node_UI::setHorizontalOffset(const int& keyHO)
{
	_hOffset = keyHO;
	Node_Block* UIelem = static_cast<Node_Block*>(_sg->at(_hOffset));
	_pos = UIelem->_pos;
	_pos.x += UIelem->_size.x;

	translateElem();
}

void Node_UI::setDepthOffset(const int& keyDO)
{
	_dOffset = keyDO;
	Node_Block* UIelem = static_cast<Node_Block*>(_sg->at(_dOffset));
	_pos.z += UIelem->_pos.z;
	_pos.z += UIelem->_size.z;

	translateElem();
}

void Node_UI::update()
{
	translateElem();
}

bool Node_UI::_checkPointCollision(glm::vec2 const& pt)
{
	glm::vec3 absPos = getGlobalTransform()[3]; // Translation from Transform

	if ((pt.x <= (absPos.x + _size.x) && (pt.x >= absPos.x)
		&& pt.y >= (absPos.y + _size.y) && (pt.y <= absPos.y)))
	{
		// Cursor enter the UI elem
		if (!_hover)
		{
			EMIT_EVENT("NODE_UI_HOVER");
			SSS::log_msg("Hover the node " + std::to_string(_key));
		}

		_hover = true;
		return true; // Pt in the UI elem
	}

	// Cursor leave the UI elem
	if (_hover)
	{
		EMIT_EVENT("NODE_UI_MOUSE_LEFT");
		SSS::log_msg("Leave the node " + std::to_string(_key));
	}

	_hover = false;
	return false;
}


/* -------------------------------------------------------
---------------------- TEXT NODES ------------------------
--------------------------------------------------------*/

void Node_Text::_register()
{
	REGISTER_EVENT("NODE_TEXT_RESIZE");
	REGISTER_EVENT("NODE_TEXT_CONTENT_UPDATE");
}

Node_Text::Node_Text(SceneGraph* p_Sg, const std::string& s, const SSS::GUI_Layout& lyt)
	:Node_UI(p_Sg)
{
	SSS::TR::Format fmt = lyt._fmt;
	fmt.charsize = 58;
	auto area = SSS::TR::Area::create();
	auto plane = TextPlane::create(SSS::GL::Texture::create(area));
	//plane->setBox(nullptr);
	_observe(*plane->getTexture());


	glm::vec4 tex_col = SSS::RGBA_f(BLACK).to_HSL();
	glm::vec4 bg_col = tex_col;

	tex_col.b = 0.3f;
	fmt.text_color = SSS::RGBA_f::from_HSL(tex_col);

	bg_col.b -= 0.15f;
	area->setClearColor(SSS::RGBA_f::from_HSL((bg_col)));
	area->setClearColor(static_cast<SSS::RGBA32>(SSS::RGBA_f{ BLACK }));
	area->setFocusable(true);
	area->setWrapping(true);
	area->setMargins(lyt._marginv, lyt._marginh);
	area->setWrappingMaxWidth(_maxStrSize);
	area->setFormat(fmt);
	area->parseString(s);

	//Create the model
	plane->translate(_pos);

	plane->setHitbox(SSS::GL::Plane::Hitbox::Full);
	model = plane;

	_size_update();

	_sg->_rd->addPlane(plane);
	_sg->emplace(this);
}

void Node_Text::_subjectUpdate(SSS::Subject const& subject, int event_id)
{
	if (_hidden)
		return;

	if (event_id == EVENT_ID("SSS_TEXTURE_CONTENT")) {
		auto [w, h] = model->getTexture()->getCurrentDimensions();
		model->setScaling(glm::vec3(static_cast<float>(std::min(w, h))));


		EMIT_EVENT("NODE_TEXT_RESIZE");
		return;
	}


	auto& texture = static_cast<SSS::GL::Texture const&>(subject);
	if (texture.getType() == SSS::GL::Texture::Type::Text &&
		texture.getTextArea() && texture.getTextArea()->isFocused())
	{
		auto area = model->getTextArea();
		if (!area) return;
		if (area->getUsedWidth() == _size.x)
			return;

		_size_update();
		EMIT_EVENT("NODE_TEXT_RESIZE");
	}
}


Node_Text::~Node_Text()
{
	clear();
}

void Node_Text::clear()
{
	_sg->_rd->removePlane(model);
	model.reset();
	EMIT_EVENT("NODE_TEXT_CONTENT_UPDATE");
}

void Node_Text::setWrappingMin(const int& min)
{
	model->getTextArea()->setWrappingMinWidth(min);
}

void Node_Text::setMaxStrSize(const int maxSize)
{
	_maxStrSize = maxSize;
}

void Node_Text::setTextColor(const  SSS::RGBA_f& col)
{
	SSS::TR::Format fmt = model->getTextArea()->getFormat();
	fmt.text_color = col;

	model->getTextArea()->setFormat(fmt);
}

void Node_Text::setBackgroundColor(const  SSS::RGBA_f& bgCol)
{
	model->getTextArea()->setClearColor(bgCol);
}

void Node_Text::_size_update()
{
	glm::vec3 const old_size = _size;
	_size = glm::vec3(0);

	model->setOffset(glm::vec3(0, -_size.y, BOX_LAYER));
	auto [w, h] = model->getTexture()->getCurrentDimensions();
	model->setScaling(glm::vec3(static_cast<float>(std::min(w, h))));
	if (auto area = model->getTextArea(); area)
	{
		w = area->getUsedWidth();
		h = area->getHeight();
	}
	_size.x = std::max(static_cast<float>(w), _size.x);
	_size.y += static_cast<float>(h);

	// Set min width
	if (_size == old_size)
		return;

	if (_size.x != old_size.x) {
		int w = static_cast<int>(_size.x);
		if (_maxStrSize - static_cast<int>(_size.x) < 10)
			w = _maxStrSize;

		if (auto area = model->getTextArea(); area)
			area->setWrappingMinWidth(w);
	}
}


void Node_Text::update()
{
	glm::mat4 transform = getGlobalTransform(); // your transformation matrix.
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(transform, scale, rotation, translation, skew, perspective);
	glm::vec3 rot = glm::eulerAngles(rotation);

	model->setTranslation(translation);
	model->setRotation(glm::degrees(rot));
}

void Node_Text::parseText(const std::string& str)
{
	model->getTextArea()->parseString(str);
}


/* -------------------------------------------------------
----------------------- SLIDERS --------------------------
--------------------------------------------------------*/

Node_Slider::~Node_Slider()
{
}

void Node_Slider::build()
{
	_min = 0;
	_max = 255;
	UIPrimitive ui;
	ui.shapeId = sdSegment;
	ui.size.x = 15;
	ui.size.y = 0;
	ui.pos = glm::vec3(100, 250, 0);
	ui.pos2 = glm::vec3(400, 250, 0);
	ui.color = glm::vec4(1.0);
	ui.blendMode = GROUP;
	prims.push_back(ui);

	ui.shapeId = sdCircle;
	ui.pos = glm::vec3(200, 250, 0);
	ui.size.x = 25;
	ui.blendMode = GROUP|SUBTRACT;
	prims.push_back(ui);

	//focus circle 
	ui.shapeId = sdCircle;
	ui.size.x = 40;
	ui.blendMode = DEFAULT;
	ui.color = glm::vec4(1.0, 1.0, 1.0, 0.5);
	prims.push_back(ui);

	ui.shapeId = sdCircle;
	ui.size.x = 20;
	ui.color = glm::vec4(1.0);
	prims.push_back(ui);

	_pos = { prims[0].pos.x - prims[3].size.x,  prims[0].pos.y - prims[3].size.x, 0.0 };
	_size = { prims[0].pos2.x - prims[0].pos.x + 2 * prims[3].size.x, 2 * prims[3].size.x, 0.0 };


	//BB
	boundingBox();
}


void Node_Slider::clear()
{

}

void Node_Slider::getCursorPos(const float& x, const float& y)
{
	float cx, cy;
	cx = std::clamp(x, prims[0].pos.x, prims[0].pos2.x);
	prims[2].pos.x = cx; 
	prims[1].pos.x = cx;
	prims[3].pos.x = cx;

	float progress = (cx - prims[0].pos.x) / (prims[0].pos2.x - prims[0].pos.x);

	_current = (_max - _min) * progress + _min;

}

bool Node_UI::checkCollision(const glm::vec3& pos)
{
	if (pos.x >= _pos.x && pos.x <= (_pos.x + _size.x)
		&& pos.y >= _pos.y  && pos.y <= (_pos.y + _size.y)) {
		return true;
	}
	return false;
}


void Node_Slider::_subjectUpdate(SSS::Subject const& subject, int event_id)
{
	if (_hidden)
		return;

	if (event_id == EVENT_ID("SSS_WINDOW_MOUSE_POSITION")) {
		const SSS::GL::Window &window = static_cast<SSS::GL::Window const&>(subject);
		int c_y, c_x;
		window.getCursorPos(c_x, c_y);

		_hover = checkCollision(glm::vec3(c_x, c_y, 0));

		if (!_focus) {
			float dist = std::clamp(glm::distance(prims[2].pos, glm::vec2(c_x, c_y)), 0.f, 2*prims[2].size.r);
			float factor = glm::smoothstep(0.f, 1.0f, 1.0f - dist / prims[2].size.r);
			prims[2].color.a = 0.5 * factor;
		}

		if (_held) {
			getCursorPos(c_x, c_y);
			prims[2].color.a = 0.6;
		}
	}

	if (event_id == EVENT_ID("SSS_WINDOW_MOUSE_INPUT")) {
		const SSS::GL::Window& window = static_cast<SSS::GL::Window const&>(subject);
		auto const& clicks = window.getClickInputs();

		int c_y, c_x;
		window.getCursorPos(c_x, c_y);
		if (clicks[GLFW_MOUSE_BUTTON_1].is_pressed()) {
			_focus = _hover;
			_held = _hover;
		}
		if (clicks[GLFW_MOUSE_BUTTON_1].is_released()) {
			_held = false;
			if (!_focus) {
				float dist = std::clamp(glm::distance(prims[2].pos, glm::vec2(c_x, c_y)), 0.f, 2 * prims[2].size.r);
				float factor = glm::smoothstep(0.f, 1.0f, 1.0f - dist / prims[2].size.r);
				prims[2].color.a = 0.5 * factor;
			}
		}

		if (_held) {
			getCursorPos(c_x, c_y);
			prims[2].color.a = 0.6;
		}
	}
}


/* -------------------------------------------------------
----------------------- TOGGLES --------------------------
--------------------------------------------------------*/


void Node_Toggle::build() {
	_radius = 40;

	glm::vec2 begin = glm::vec2(810, 250);
	glm::vec2 end = glm::vec2(810 +  _radius*0.6, 250);
	glm::vec4 color = glm::vec4(0.87, 0.53, 0.56, 1.0);
	glm::vec4 emptyColor = glm::vec4(0.118, 0.118, 0.118, 1.0);
	glm::vec4 background = glm::vec4(0.27f, 0.27f, 0.27f, 1.0f);

	UIPrimitive toggleButton;
	// Background
	toggleButton.shapeId = sdSegment;
	toggleButton.pos = begin;
	toggleButton.pos2 = end;
	toggleButton.cornerRadius = 0.02f;
	toggleButton.size.r = _radius;
	//toggleButton.borderWidth = 1.0;
	//toggleButton.color = mix(emptyColor, color, (float)_active);
	toggleButton.color = glm::vec4(0.0);
	toggleButton.border = glm::vec4(1.0);
	toggleButton.borderWidth = 5.0;
	toggleButton.blendMode = 0;
	prims.push_back(toggleButton);

	// Button
	toggleButton.shapeId = sdCircle;
	toggleButton.cornerRadius = 0;
	toggleButton.pos = glm::mix(begin, end, (float)_active);
	toggleButton.size.r = _radius * 0.35;
	toggleButton.color = glm::vec4(1.0);

	toggleButton.borderWidth = 0.0;

	//toggleButton.color = background;
	prims.push_back(toggleButton);

	_size = glm::vec3(end.x - begin.x + _radius, _radius, 0);
	_pos = glm::vec3(begin.x - _radius/2, begin.y - _radius/2, 0.f);

	//BB
	boundingBox();
}

void Node_Toggle::_subjectUpdate(SSS::Subject const& subject, int event_id) {
	if (_hidden)
		return;

	if (event_id == EVENT_ID("SSS_WINDOW_MOUSE_POSITION")) {
		const SSS::GL::Window& window = static_cast<SSS::GL::Window const&>(subject);
		int c_y, c_x;
		window.getCursorPos(c_x, c_y);

		_hover = checkCollision(glm::vec3(c_x, c_y, 0));
		return;
	}

	if (event_id == EVENT_ID("SSS_WINDOW_MOUSE_INPUT")) {
		const SSS::GL::Window& window = static_cast<SSS::GL::Window const&>(subject);
		auto const& clicks = window.getClickInputs();

		int c_y, c_x;
		window.getCursorPos(c_x, c_y);
		if (clicks[GLFW_MOUSE_BUTTON_1].is_pressed() && _hover) {
			_focus = _hover;
			_active ^= true;
			prims[1].pos = glm::vec3(prims[0].pos,0) + glm::vec3((_radius * 0.6f)*(float)(_active), 0,0);
		}
		return;
	}
}


/* -------------------------------------------------------
----------------------- TOGGLES --------------------------
--------------------------------------------------------*/


Node_CheckBox::Node_CheckBox(const glm::vec3& pos, const float& h)
{
	_pos = pos;
	_radius = h;
	_borderWidth = 2.0;
	build();
}

void Node_CheckBox::build() {

	prims.clear();
	float factor = 0.6f;
	glm::vec2 begin = _pos;
	glm::vec2 end = begin + glm::vec2(_radius, 0);
	glm::vec4 color = glm::vec4(0.87, 0.53, 0.56, 1.0);
	glm::vec4 emptyColor = glm::vec4(0.118, 0.118, 0.118, 1.0);
	glm::vec4 background = glm::vec4(0.27f, 0.27f, 0.27f, 1.0f);

	UIPrimitive toggleButton;
	// Background
	toggleButton.shapeId = sdOrientedBox;
	toggleButton.pos = begin;
	toggleButton.pos2 = end;
	toggleButton.size.r = _radius;
	toggleButton.color = glm::vec4(1.0);
	toggleButton.border = glm::vec4(1.0);
	toggleButton.borderWidth = 0;
	toggleButton.blendMode = GROUP;
	prims.push_back(toggleButton);

	toggleButton.shapeId = sdOrientedBox;
	toggleButton.pos.x = begin.x + _borderWidth;
	toggleButton.pos2.x = end.x - _borderWidth;
	toggleButton.size.r = _radius - 2* _borderWidth;
	toggleButton.blendMode = SUBTRACT|GROUP;
	prims.push_back(toggleButton);

	// Button
	toggleButton.shapeId = sdOrientedBox;
	toggleButton.cornerRadius = 0;
	toggleButton.pos = begin + glm::vec2(2*_borderWidth,0);
	toggleButton.pos2 = end - glm::vec2(2*_borderWidth, 0);
	toggleButton.size.r = _radius - 4*_borderWidth;
	toggleButton.color = glm::mix(glm::vec4(0.0), glm::vec4(1.0), (float)_active);;
	toggleButton.blendMode = DEFAULT;
	toggleButton.borderWidth = 0.0;

	//toggleButton.color = background;
	prims.push_back(toggleButton);

	_size = glm::vec3(_radius, _radius, 0);
	_pos = glm::vec3(begin.x, begin.y - _radius / 2, 0.f);

	//BB
	//boundingBox();
}

void Node_CheckBox::_subjectUpdate(SSS::Subject const& subject, int event_id) {
	if (_hidden)
		return;

	if (event_id == EVENT_ID("SSS_WINDOW_MOUSE_POSITION")) {
		const SSS::GL::Window& window = static_cast<SSS::GL::Window const&>(subject);
		int c_y, c_x;
		window.getCursorPos(c_x, c_y);

		_hover = checkCollision(glm::vec3(c_x, c_y, 0));
		return;
	}

	if (event_id == EVENT_ID("SSS_WINDOW_MOUSE_INPUT")) {
		const SSS::GL::Window& window = static_cast<SSS::GL::Window const&>(subject);
		auto const& clicks = window.getClickInputs();

		int c_y, c_x;
		window.getCursorPos(c_x, c_y);
		if (clicks[GLFW_MOUSE_BUTTON_1].is_pressed() && _hover) {
			_focus = _hover;
			_active ^= true;
			prims[2].color = glm::mix(glm::vec4(0.0),  glm::vec4(1.0), (float)_active);
		}
		return;
	}
}

/* -------------------------------------------------------
--------------------- RADIO BUTTONS ----------------------
--------------------------------------------------------*/


void Node_RadioButton::_register()
{
	REGISTER_EVENT("SSS_RADIO_TOOK_FOCUS");
}

Node_RadioButton::Node_RadioButton(const glm::vec2& pos, const float& h) {
	_pos = glm::vec3(pos, 0);
	_radius = h*0.5;
	_borderWidth = 4.0;
	build(_pos);
}

void Node_RadioButton::observeRadio(Node_RadioButton &subject)
{
	_observe(subject);

	if (subject.isActive()) {
		_active = false;
	}
	build(_pos);
}

void Node_RadioButton::build(const glm::vec2& begin) {
	prims.clear();
	float factor = 0.6f;

	glm::vec4 color = glm::vec4(0.87, 0.53, 0.56, 1.0);
	glm::vec4 emptyColor = glm::vec4(0.118, 0.118, 0.118, 1.0);
	glm::vec4 background = glm::vec4(0.27f, 0.27f, 0.27f, 1.0f);

	UIPrimitive radio;
	// Background
	radio.shapeId = sdCircle;
	radio.pos = begin;
	radio.size.r = _radius;
	radio.color = glm::vec4(0.0);
	radio.border = glm::vec4(1.0);
	radio.borderWidth = _borderWidth;
	radio.blendMode = DEFAULT;
	prims.push_back(radio);

	// Button
	radio.shapeId = sdCircle;
	radio.size.r = std::max(_radius - _borderWidth, _radius*factor);
	radio.color = glm::mix(glm::vec4(0.0), glm::vec4(1.0), (float)_active);
	radio.borderWidth = 0.0;
	prims.push_back(radio);

	_pos = glm::vec3(begin.x - _radius , begin.y - _radius, 0.f);
	_size = glm::vec3(_radius * 2, _radius * 2, 0);

	boundingBox();
}

void Node_RadioButton::_subjectUpdate(SSS::Subject const& subject, int event_id) {
	if (_hidden)
		return;

	if (event_id == EVENT_ID("SSS_RADIO_TOOK_FOCUS")) {
		_focus = false;
		_active = false;
		prims[1].color = glm::mix(glm::vec4(0.0), glm::vec4(1.0), (float)_active);
		return;
	}

	if (event_id == EVENT_ID("SSS_WINDOW_MOUSE_POSITION")) {
		const SSS::GL::Window& window = static_cast<SSS::GL::Window const&>(subject);
		int c_y, c_x;
		window.getCursorPos(c_x, c_y);

		_hover = checkCollision(glm::vec3(c_x, c_y, 0));
		return;
	}

	if (event_id == EVENT_ID("SSS_WINDOW_MOUSE_INPUT")) {
		const SSS::GL::Window& window = static_cast<SSS::GL::Window const&>(subject);
		auto const& clicks = window.getClickInputs();

		int c_y, c_x;
		window.getCursorPos(c_x, c_y);
		if (clicks[GLFW_MOUSE_BUTTON_1].is_pressed() && _hover) {
			_focus = _hover;
			_active = true;
			EMIT_EVENT("SSS_RADIO_TOOK_FOCUS");
			prims[1].color = glm::mix(glm::vec4(0.0), glm::vec4(1.0), (float)_active);
		}
		return;
	}
}