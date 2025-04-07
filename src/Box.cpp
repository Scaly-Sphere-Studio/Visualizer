#include "Box.h"
#include "visualizer.h"

#define FLAG_ID                 0
#define FLAG_TEXT               1

#define TEXT_MAX_WIDTH          600

std::map<uint16_t, Tags>Box::tags_list{};
std::map<std::string, GUI_Layout> Box::layout_map{};
glm::vec2 Box::minsize = glm::vec2{ 150,75 };


glm::mat4 BoxPlane::_getTranslationMat4() const {
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

void BoxPlane::setOffset(glm::vec3 offset) {
    _offset = offset;
    _computeModelMat4();
}

Box::~Box()
{
    link_to.clear();
    link_from.clear();
    model.clear();
}

static bool _checkCollision(glm::vec2 const& r1p, glm::vec2 const& r1s, glm::vec2 const& r2p, glm::vec2 const& r2s)
{
    if (r1p.y +  r1s.y >= r2p.y &&      // r1 top edge past r2 bottom
        r1p.y <= r2p.y +  r2s.y &&      // r1 bottom edge past r2 top
        r1p.x +  r1s.x >= r2p.x &&      // r1 right edge past r2 left
        r1p.x <= r2p.x +  r2s.x)        // r1 left edge past r2 right
    {
        LOG_MSG("Collision !!");
        return true;
    }
    return false;
}

bool Box::checkCollision(glm::vec2 const& r2p, glm::vec2 const& r2s)
{
    return _checkCollision({ _pos.x, _pos.y - _size.y }, _size, r2p, r2s);
}

bool Box::checkCollision(std::shared_ptr<SSS::GL::PlaneBase> plane)
{
    glm::vec2 const pos = plane->getTranslation();
    glm::vec2 const size = plane->getScaling();

    return checkCollision(pos - (size / 2.f), size);
}


glm::vec3 Box::center() const
{
    return _pos + glm::vec3(_size.x / 2.f, _size.y / -2.f, 0);
}

void Box::setPos(glm::vec3 pos)
{
    _pos = pos;
    for (auto& plane : model) {
        plane->setTranslation(_pos);
    }
}

void Box::setColor(glm::vec4 color)
{
    _color = color;
    for (auto& plane : model) {
        auto texture = plane->getTexture();
        if (!texture)
            continue;
        auto area = texture->getTextArea();
        if (!area)
            continue;
        area->setClearColor(rgb_to_int32t(_color));
    }
}


void Box::set_text_data(const Text_data& td)
{
    _td = td;
    create_box();
}

bool Box::isHovered() const noexcept
{
    return std::any_of(model.cbegin(), model.cend(),
        [](BoxPlane::Shared p) { return p->isHovered(); });
}

bool Box::isClicked() const noexcept
{
    return std::any_of(model.cbegin(), model.cend(),
        [](BoxPlane::Shared p) { return p->isClicked(); });
}

bool Box::isHeld() const noexcept
{
    return std::any_of(model.cbegin(), model.cend(),
        [](BoxPlane::Shared p) { return p->isHeld(); });
}

void Box::create_box()
{
    _size.y = 0;
    for (auto& a : model)
        if (a)
            _ignore(*a->getTexture());
    Visualizer::get().box_renderer->removePlanes(model);
    model.clear();

    //Brightning the color
    glm::vec4 factor = (glm::vec4(1.f) - _color) * glm::vec4(0.2f);

    // Create the model
    _create_part(_td.text_ID, Box::layout_map["ID"], FLAG_ID);
    // Text
    _create_part(_td.text, Box::layout_map["TEXT"], FLAG_TEXT);
    // Comment text
    if (!_td.comment.empty())
        _create_part(_td.comment, Box::layout_map["COMMENT"], FLAG_TEXT);
    // tags
    if (!tags.empty())
        _create_part("TODO: tags", Box::layout_map["TEXT"], FLAG_TEXT);

    _size_update();

    //Tags
    if (tags.size() > 0) {
        model.reserve(tags.size() * 2);
        for (size_t i = 0; i < tags.size(); ++i) {
            model.insert(model.end(), tags_list[tags[i]]._model.begin(), tags_list[tags[i]]._model.end());
        }
    }

    Visualizer::get().box_renderer->addPlanes(model);
}

#define PARTICLE_VERTICES       0
#define PARTICLE_SIZE           1
#define PARTICLE_COLOR          2
#define PARTICLE_POSITION       3
#define PARTICLE_UV             4
#define ID_TEXTURE              5
//Transformations
#define PARTICLE_TRANSLATE      6
#define PARTICLE_SCALE          7
#define PARTICLE_ROTATE         8


Tags::Tags()
{
    _weight = 1;
}

Tags::Tags(std::string _name, std::string hex, uint32_t weight)
{
    //int char_size = 12;
    //_size = { char_size * _name.size() + 5, char_size * 1.5f};
    ////Center the box around the cursor
    //_pos = { 0.0f, 0.0f, 0.0f };
    //_color = hex_to_rgb(hex);
    //_weight = weight;


    //// Create text area & gl texture
    //auto& area = SSS::TR::Area::create((int)_size.x, (int)_size.y);
    //auto fmt = area.getFormat();
    ////fmt.charsize = (int)_size.y / 3;
    //fmt.charsize = char_size;
    //fmt.has_outline = false;
    //fmt.outline_size = 2;
    //fmt.text_color = 0x000000;
    //area.setFormat(fmt);
    //area.parseString(_name);

    ////Create the model
    //_model.emplace_back(_pos, _size, glm::vec4(_color));
    //_model.emplace_back(_pos + glm::vec3{1,2,0}, _size, glm::vec4(0))
    //    ._sss_texture = SSS::GL::Texture::create(area);
}

Tags::~Tags()
{
    _model.clear();
}

void Box::_create_part(std::string s, const GUI_Layout& layout, int flag)
{
    SSS::TR::Format fmt = layout._fmt;
    auto area = SSS::TR::Area::create();
    auto plane = BoxPlane::create(SSS::GL::Texture::create(area));
    plane->setBox(weak_from_this());
    _observe(*plane->getTexture());

    if (flag == FLAG_ID) {
        glm::vec4 tex_col = rgb_to_hsl(_color),
                  bg_col = tex_col;

        tex_col.b = 0.3f;
        fmt.text_color.rgb = rgb_to_int32t(hsl_to_rgb(tex_col));
        
        bg_col.b -= 0.15f;
        area->setClearColor(rgb_to_int32t(hsl_to_rgb(bg_col)));
        // TODO: Update TR pour que le texte soit au milieu de la "ligne" et non en haut
        //fmt.line_spacing = 1.f;
    }
    else {
        area->setClearColor(rgb_to_int32t(_color));
        area->setFocusable(true);
        area->setWrapping(true);
    }

    area->setMargins(layout._marginv, layout._marginh);
    area->setWrappingMaxWidth(TEXT_MAX_WIDTH);
    area->setFormat(fmt);
    area->parseString(s);

    //Create the model
    plane->translate(_pos);
    
    plane->setHitbox(SSS::GL::Plane::Hitbox::Full);
    model.emplace_back(plane);
}

void Box::_subjectUpdate(SSS::Subject const& subject, int event_id)
{
    if (event_id == SSS::GL::Texture::Resize) {
        _size_update();
        return;
    }
    auto& texture = static_cast<SSS::GL::Texture const&>(subject);
    if (texture.getType() == SSS::GL::Texture::Type::Text &&
        texture.getTextArea() && texture.getTextArea()->isFocused())
    {
        for (auto const& p : model) {
            auto area = p->getTextArea();
            if (!area) continue;
            if (area->getUsedWidth() == _size.x)
                return;
        }
        _size_update();
    }
}

void Box::_size_update() try
{
    glm::vec2 const old_size = _size;
    _size = glm::vec2(0);
    for (BoxPlane::Shared& p : model) {
        p->setOffset(glm::vec3(0, -_size.y, BOX_LAYER));
        auto [w, h] = p->getTexture()->getCurrentDimensions();
        p->setScaling(glm::vec3(static_cast<float>(std::min(w, h))));
        if (auto area = p->getTextArea(); area)
            w = area->getUsedWidth();
        _size.x = std::max(static_cast<float>(w), _size.x);
        _size.y += static_cast<float>(h);
    }
    // Set min width
    if (_size == old_size)
        return;
    if (_size.x != old_size.x) {
        int w = static_cast<int>(_size.x);
        if (TEXT_MAX_WIDTH - static_cast<int>(_size.x) < 10)
            w = TEXT_MAX_WIDTH;
        for (BoxPlane::Shared& p : model) {
            if (auto area = p->getTextArea(); area)
                area->setWrappingMinWidth(w);
        }
    }
    Visualizer::get().link_box(*this);
}
CATCH_AND_LOG_METHOD_EXC;