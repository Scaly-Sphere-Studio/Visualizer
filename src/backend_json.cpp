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
    {"ID", t._id},
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
    j.at("ID").get_to(t._id);
    j.at("COLOR").get_to(t._color);
    j.at("POSITION").get_to(t._pos);
    j.at("SIZE").get_to(t._size);
    j.at("TAGS").get_to(t.tags);
    j.at("LINK_TO").get_to(t.link_to);
    j.at("LINK_FROM").get_to(t.link_from);


    t.create_box();
}
