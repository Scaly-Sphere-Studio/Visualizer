#include "Box.h"

#define FLAG_ID                 0
#define FLAG_TEXT               1

#define TEXT_MAX_WIDTH          600

std::map<uint16_t, Tags>Box::tags_list{};
std::map<std::string, GUI_Layout> Box::layout_map{};
glm::vec2 Box::minsize = glm::vec2{ 150,75 };

Particle::Particle()
{
    _pos = glm::vec3(0);
    _size = glm::vec2(50.f, 50.f);
    _color = glm::vec4(0);

    translation = glm::vec3(0,0,0);
}

Particle::Particle(glm::vec3 pos, glm::vec2 s, glm::vec4 _col) :
    _pos(pos), _size(s), _color(_col)
{
    translation = glm::vec3(0, 0, 0);
}


bool Particle::check_collision(glm::vec3 const& c_pos)
{
    //Check if a point is hovering the box
    //Point to Box Collision test
    if (((c_pos.x > _pos.x) && (c_pos.x < static_cast<double>(_pos.x) + static_cast<double>(_size.x))) &&
        ((c_pos.y < _pos.y) && (c_pos.y > static_cast<double>(_pos.y) - static_cast<double>(_size.y)))) {
        return true;
    }
    return false;
}

bool Particle::check_collision(Particle p)
{
    glm::vec2 delta = glm::vec3(glm::abs(p.center() - this->center()));
    glm::vec2 sum = (glm::abs(p._size) + this->_size) / 2.f;

    if (delta.x < sum.x && delta.y < sum.y) {
        return true;
    }
    return false;
}

glm::vec3 Particle::center()
{
    return _pos + glm::vec3(_size.x /2.0, -_size.y /2.0, 0);
}

glm::vec2 Particle::center2D()
{
    return glm::vec2(center());
}

glm::vec3 Particle::centerZ0()
{
    return glm::vec3(center2D(), 0);
}


glm::mat4 BoxPlane::_getTranslationMat4() const {
    return glm::translate(ModelBase::_getTranslationMat4(), _offset);
}

void BoxPlane::setOffset(glm::vec3 offset) {
    _offset = offset;
    _computeModelMat4();
}

Box::Box(glm::vec3 pos, glm::vec2 s, std::string hex)
{
    _size = s;
    //Center the box around the cursor
    _pos = pos;
    _color = hex_to_rgb(hex);
    _id = hex;

    create_box();
}

Box::~Box()
{
    link_to.clear();
    link_from.clear();
    model.clear();
}


void Box::set_selected_col(std::string hex)
{
}

void Box::set_col(std::string hex)
{
}

void Box::set_text_data(const Text_data& td)
{
    _td = td;
    create_box();
}

void Box::create_box()
{
    _size.y = 0;
    curs_pos = glm::vec2(0);
    model.clear();

    //Brightning the color
    glm::vec4 factor = (glm::vec4(1.f) - _color) * glm::vec4(0.2f);

    // Create the model
    _create_part(_td.text_ID, Box::layout_map["ID"], ID_TEXT_LAYER, FLAG_ID);
    // Text
    _create_part(_td.text, Box::layout_map["TEXT"], MAIN_TEXT_LAYER, FLAG_TEXT);
    // Comment text
    if (!_td.comment.empty())
        _create_part(_td.comment, Box::layout_map["COMMENT"], COMMENT_TEXT_LAYER, FLAG_TEXT);
    // tags
    if (!tags.empty())
        _create_part("TODO: tags", Box::layout_map["TEXT"], TAGS_TEXT_LAYER, FLAG_TEXT);

    for (BoxPlane::Shared& p : model) {
        p->getTexture()->getTextArea()->setWidth(static_cast<int>(_size.x));
        p->setOffset(p->getOffset() += glm::vec3(_size.x / 2.f, 0, 0));
    }
        
    //Tags
    if (tags.size() > 0) {
        model.reserve(tags.size() * 2);
        for (size_t i = 0; i < tags.size(); ++i) {
            model.insert(model.end(), tags_list[tags[i]]._model.begin(), tags_list[tags[i]]._model.end());
        }
    }

    update();
}

SSS::GL::Texture::Shared Box::check_text_selection(glm::vec3 const& c_pos)
{
    //for (Particle p : text_model) {
    //    if (p.check_collision(c_pos) && p._sss_texture) {
    //        p._sss_texture->getTextArea()->setFocus(true);
    //        return p._sss_texture;
    //    }
    //}

    return nullptr;
}

void Box::update()
{
    for (auto& plane : model) {
        plane->setTranslation(_pos);
    }
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

void Box::_create_part(std::string s, const GUI_Layout& layout, float layer, int flag)
{
    SSS::TR::Format fmt = layout._fmt;
    auto& area = SSS::TR::Area::create();

    if (flag == FLAG_ID) {
        glm::vec4 tex_col = rgb_to_hsl(_color),
                  bg_col = tex_col;

        tex_col.b = 0.3f;
        fmt.text_color.rgb = rgb_to_int32t(hsl_to_rgb(tex_col));
        
        bg_col.b -= 0.15f;
        area.setClearColor(rgb_to_int32t(hsl_to_rgb(bg_col)));
        // TODO: Update TR pour que le texte soit au milieu de la "ligne" et non en haut
        //fmt.line_spacing = 1.f;
    }
    else {
        area.setClearColor(rgb_to_int32t(_color));
    }

    area.setMargins(layout._marginv, layout._marginh);
    area.setWrappingMaxWidth(TEXT_MAX_WIDTH);
    area.setFormat(fmt);
    area.parseString(s);
    auto [x, y] = area.getDimensions();

    //Create the model
    auto plane = BoxPlane::create(SSS::GL::Texture::create(area));
    plane->translate(_pos);
    plane->setOffset(glm::vec3(0, curs_pos.y - static_cast<float>(y) / 2.f, layer));
    plane->scale(static_cast<float>(std::min(x, y)));
    model.emplace_back(plane);

    _size.x = std::max(_size.x, static_cast<float>(x));
    _size.y += y;
    curs_pos.y -= y;
}
