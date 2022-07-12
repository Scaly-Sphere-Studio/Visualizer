#include "backend_json.h"

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


    //Brightning the color
    glm::vec4 factor = (glm::vec4(1.f) - t._color) * glm::vec4(0.2f);

    //Create the model
    t.model.emplace_back(t._pos, t._size, t._color);
    t.model.emplace_back(t._pos, glm::vec2(t._size.x - 2, t._size.y / 3), glm::vec4(t._color + factor));

    // Create text area & gl texture
    auto const& area = SSS::TR::Area::create((int)t._size.x, (int)t._size.y);
    auto fmt = area->getFormat();
    fmt.style.charsize = (int)t._size.y / 3;
    fmt.style.has_outline = true;
    fmt.style.outline_size = 20;
    area->setFormat(fmt);
    area->parseString(rgb_to_hex(t._color));

    auto const& texture = SSS::GL::Texture::create();
    texture->setTextAreaID(area->getID());
    texture->setType(SSS::GL::Texture::Type::Text);

    t.model.emplace_back(t._pos, t._size, glm::vec4(0))._sss_tex_id = texture->getID();
}
