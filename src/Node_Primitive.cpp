#include "Node_Primitive.h"

#include "scenegraph.h"

#define BLACK glm::vec4(0,0,0,1)
#define WHITE glm::vec4(1,1,1,1)

glm::mat4 TextPlane::_getTranslationMat4() const {
	glm::vec3 offset = _offset;
	auto texture = getTexture();
	if (texture) {
		auto const [w, h] = getTexture()->getCurrentDimensions();
		float const x = static_cast<float>(w) / 2.f;
		float const y = static_cast<float>(-h) / 2.f;
		offset += glm::vec3(x, y, 0);
	}
	return glm::translate(ModelBase::_getTranslationMat4(), offset);
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

	if (event_id == SSS::EventList::Resize) {
		auto [w, h] = model->getTexture()->getCurrentDimensions();
		model->setScaling(glm::vec3(static_cast<float>(std::min(w, h))));

		_notifyObservers(SSS::EventList::Resize);
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
		_notifyObservers(SSS::EventList::Resize);
	}
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

Node_Block::Node_Block(SceneGraph* p_Sg): Node(p_Sg)
{
	_pos = glm::vec3{ 0, 0, 0 };
	_size = glm::vec3{ 0, 0, 0 };
}

glm::mat4 Node_Block::getLocalTransform()
{
	//Rotation along the z axis with _pos translation
	glm::mat4 mat = glm::mat4(1.0);
	mat = glm::translate(mat, _pos);
	mat *= glm::rotate(rotation, glm::vec3(0, 0, 1));
	return mat;
}

bool Node_Block::checkCollision2D(glm::vec2 np2, glm::vec2 ns2)
{
	//node pos np, node size ns
	glm::vec3 globPos = getGlobalTransform()[3];
	

	if (globPos.y + _size.y >= np2.y &&      // r1 top edge past r2 bottom
		globPos.y <= np2.y + ns2.y &&      // r1 bottom edge past r2 top
		globPos.x + _size.x >= np2.x &&      // r1 right edge past r2 left
		globPos.x <= np2.x + ns2.x)        // r1 left edge past r2 right
	{
		LOG_MSG("Collision !!");
		return true;
	}

	return false;
}

glm::vec3 Node_Block::center()
{
	return glm::vec3(_pos) + glm::vec3(_size.x / 2.f, _size.y / -2.f, _size.z / 2.f);
}

glm::mat4 Node_Block::getGlobalTransform()
{
	if (_inherited_transform == false || _parent == 0)
		return getLocalTransform();

	Node_Block* p_pNode = (Node_Block*)_sg->at(_parent);
	return p_pNode->getGlobalTransform() * getLocalTransform();;
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

	if ((pt.x < (absPos.x + _size.x) && (pt.x > absPos.x) 
		&& pt.y > (absPos.y + _size.y) && (pt.y < absPos.y)))
	{
		// Cursor enter the UI elem
		if (!_hover)
		{
			_notifyObservers(SSS::EventList::Hover);
			SSS::log_msg("Hover the node " + std::to_string(_key));
		}

		_hover = true;
		return true; // Pt in the UI elem
	}

	// Cursor leave the UI elem
	if (_hover) 
	{
		_notifyObservers(SSS::EventList::Leave);
		SSS::log_msg("Leave the node "+ std::to_string(_key));
	}

	_hover = false;
	return false;
}
