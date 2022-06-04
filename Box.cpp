#include "Box.h"

std::vector<testBox>Box::box_batch{};
GLuint Box::box_shader = 0;


Box::Box(float width, float height, std::string hex)
{

    pos = glm::vec3(width - _w / 2, height + _h / 2, 0.0);
    base_color = hex_to_rgb(hex);


    glm::vec3 newpos = glm::vec3(width - _w / 2, height + _h / 2, 3.9);
    //Brightning the color
    glm::vec3 factor = (glm::vec3(1) - base_color) * glm::vec3(0.2);



    model.emplace_back(pos, glm::vec2(_w, _h), glm::vec4(base_color, 1.0f));
    model.emplace_back(pos, glm::vec2(_w - 2, _h / 3), glm::vec4(base_color + factor, 1.0f));



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
    base_color = hex_to_rgb(hex);
}


bool Box::check_collision(double x, double y)
{
    //Check if a point is hovering the box
    //Point to Box Collision test
    if (((x > pos.x) && (x < static_cast<double>(pos.x) + static_cast<double>(_w))) &&
        ((y < pos.y) && (y > static_cast<double>(pos.y) - static_cast<double>(_h)))) {

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
        model[i].pos = pos;
    }
}

glm::vec3 Box::center()
{
    return glm::vec3();
}

testBox::testBox(glm::vec3 _pos, glm::vec2 s, glm::vec4 _col) :
    pos(_pos), size(s), color(_col)
{
}
