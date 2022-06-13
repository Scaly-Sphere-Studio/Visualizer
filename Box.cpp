#include "Box.h"

std::vector<testBox>Box::box_batch{};
GLuint Box::box_shader = 0;


Box::Box(glm::vec3 pos, glm::vec2 s, std::string hex)
{
    _size = s;
    //Center the box around the cursor
    _pos = pos + glm::vec3{ -_size.x / 2.0f , _size.y / 2.0f, rand_float() };
    _color = hex_to_rgb(hex);

    //Brightning the color
    glm::vec4 factor = (glm::vec4(1) - _color) * glm::vec4(0.2);

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


bool Box::check_collision(double x, double y)
{
    //Check if a point is hovering the box
    //Point to Box Collision test
    if (((x > _pos.x) && (x < static_cast<double>(_pos.x) + static_cast<double>(_size.x))) &&
        ((y < _pos.y) && (y > static_cast<double>(_pos.y) - static_cast<double>(_size.y)))) {

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
