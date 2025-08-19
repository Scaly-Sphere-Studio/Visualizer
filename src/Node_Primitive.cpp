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
	//area->setClearColor(static_cast<SSS::RGBA32>(SSS::RGBA_f{ BLACK }));
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

	if (event_id == SSS::GL::Texture::Resize) {
		auto [w, h] = model->getTexture()->getCurrentDimensions();
		model->setScaling(glm::vec3(static_cast<float>(std::min(w, h))));

		_notifyObservers(event_id);
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
	}
}

void Node_Text::setMaxStrSize(const int maxSize)
{
	_maxStrSize = maxSize;
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

void Node_Block::_subjectUpdate(SSS::Subject const& subject, int event_id)
{
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
