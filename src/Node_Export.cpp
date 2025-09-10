
#include "Node_Export.h"
#include "Text_data.h"
#include <SSS/commons.hpp>
#include <nlohmann/json.hpp>
#include "backend_json.h"



void to_json(nlohmann::json& j, const Export_Node_Box& t)
{
    j = nlohmann::json{
    {"ID"       , t.id},
    {"COLOR"    , t.color.to_Hex()},
    {"POSITION" , t.pos} };

    if (t.tags.size())j["TAGS"] = t.tags;
    if (t.link_to.size())j["LINK_TO"]       = t.link_to;
    if (t.link_from.size())j["LINK_FROM"]   = t.link_from;
}

void from_json(const nlohmann::json& j, Export_Node_Box& t)
{
    j.at("ID").get_to(t.id);
    std::string hex;
    j.at("COLOR").get_to(hex);
    t.color = SSS::RGBA_f(hex);
    t.pos = j.at("POSITION");

    //optional fields
    if (j.contains("TAGS") && !j["TAGS"].is_null()) {
        j.at("TAGS").get_to(t.tags);
    }
    if (j.contains("LINK_TO") && !j["LINK_TO"].is_null()) {
        j.at("LINK_TO").get_to(t.link_to);
    }
    if (j.contains("LINK_FROM") && !j["LINK_FROM"].is_null()) {
        j.at("LINK_FROM").get_to(t.link_from);
    }
}
