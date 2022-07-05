#include "Box.h"

std::vector<testBox>Box::box_batch{};

Box::Box(glm::vec3 pos, glm::vec2 s, std::string hex)
{
    _size = s;
    //Center the box around the cursor
    _pos = pos + glm::vec3{ -_size.x / 2.0f , _size.y / 2.0f, rand_float() };
    _color = hex_to_rgb(hex);

    //Brightning the color
    glm::vec4 factor = (glm::vec4(1.f) - _color) * glm::vec4(0.2f);

    //Create the model
    model.emplace_back(_pos, _size, glm::vec4(_color));
    model.emplace_back(_pos, glm::vec2(_size.x - 2, _size.y / 3), glm::vec4(_color + factor));
}

Box::~Box()
{
    link_to.clear();
    link_from.clear();
}


void Box::set_selected_col(std::string hex)
{
    selected_color = hex_to_rgb(hex);
}

void Box::set_col(std::string hex)
{
    _color = hex_to_rgb(hex);
}


bool Box::check_collision(glm::vec3 const& c_pos)
{
    //Check if a point is hovering the box
    //Point to Box Collision test
    if (((c_pos.x > _pos.x) && (c_pos.x < static_cast<double>(_pos.x) + static_cast<double>(_size.x))) &&
        ((c_pos.y < _pos.y) && (c_pos.y > static_cast<double>(_pos.y) - static_cast<double>(_size.y)))) {

        if (!_hovered) {
            //switch selected state
            _hovered ^= 1;
        }
        return true;
    }

    if (_hovered) {

        _hovered ^= 1;
    }

    return false;
}

void Box::create_box()
{
    set_selected_col("C32530");
}

void Box::update()
{
    for (size_t i = 0; i < model.size(); i++) {
        model[i]._pos = _pos;
    }
}

glm::vec3 Box::center()
{

    return _pos + glm::vec3(_size.x /2.0, -_size.y /2.0, 0);
}

testBox::testBox()
{
    _pos = glm::vec3{ 0 };
    _size = glm::vec2{ 0 };
    _color = glm::vec4{ 0 };
}

testBox::testBox(glm::vec3 _pos, glm::vec2 s, glm::vec4 _col) :
    _pos(_pos), _size(s), _color(_col)
{
}

void to_json(nlohmann::json& j, const Box& t)
{
    j = nlohmann::json{
    {"ID", t.id},
    {"COLOR", t._color},
    {"POSITION", t._pos},
    {"SIZE", t._size},
    {"TAGS", t.tags},
    {"LINK_TO", t.link_to},
    {"LINK_FROM", t.link_from},
    };
}

void from_json(const nlohmann::json& j, Box& t)
{
    j.at("ID").get_to(t.id);
    j.at("COLOR").get_to(t._color);
    j.at("POSITION").get_to(t._pos);
    j.at("SIZE").get_to(t._size);
    j.at("TAGS").get_to(t.tags);
    j.at("LINK_TO").get_to(t.link_to);
    j.at("LINK_FROM").get_to(t.link_from);
}

void glm::to_json(nlohmann::json& j, const vec2& t)
{
    j = nlohmann::json{
        {"X", t.x},
        {"Y", t.y},
    };
}

void glm::from_json(const nlohmann::json& j, vec2& t)
{
    j.at("X").get_to(t.x);
    j.at("Y").get_to(t.y);
}

void glm::to_json(nlohmann::json& j, const vec3& t)
{
    j = nlohmann::json{
    {"X", t.x},
    {"Y", t.y},
    {"Z", t.z}
    };
}

void glm::from_json(const nlohmann::json& j, vec3& t)
{
    j.at("X").get_to(t.x);
    j.at("Y").get_to(t.y);
    j.at("Z").get_to(t.z);
}

void glm::to_json(nlohmann::json& j, const vec4& t)
{
    j = nlohmann::json{
    {"X", t.x},
    {"Y", t.y},
    {"Z", t.z},
    {"W", t.w}
    };
}

void glm::from_json(const nlohmann::json& j, vec4& t)
{
    j.at("X").get_to(t.x);
    j.at("Y").get_to(t.y);
    j.at("Z").get_to(t.z);
    j.at("W").get_to(t.w);
}
