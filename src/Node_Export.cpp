
#include "Node_Export.h"
#include "Text_data.h"
#include <SSS/commons.hpp>
#include <nlohmann/json.hpp>




void to_json(nlohmann::json& j, const Export_Node_Box& t)
{
    j = nlohmann::json{
    {"ID"       , t.id},
    {"COLOR"    , t.color.to_Hex()},
    {"POSITION" , t.pos} };

    //if (t.tags != std::nullopt)j["TAGS"]             = t.tags;
    //if (t.link_to != std::nullopt)j["LINK_TO"]       = t.link_to;
    //if (t.link_from != std::nullopt)j["LINK_FROM"]   = t.link_from;
}

void from_json(const nlohmann::json& j, Export_Node_Box& t)
{
    j.at("ID").get_to(t.id);
    std::string hex;
    j.at("COLOR").get_to(hex);
    t.color = SSS::RGBA_f(hex);
    j.at("POSITION").get_to(t.pos);

    // optional fields
    //if (j.contains("TAGS") && !j["TAGS"].is_null()) {
    //    j.at("TAGS").get_to(t.tags);
    //}
    //else { t.tags = {}; }
    //if (j.contains("LINK_TO") && !j["LINK_TO"].is_null()) {
    //    j.at("LINK_TO").get_to(t.tags);
    //}
    //else { t.link_to = {}; }
    //if (j.contains("LINK_FROM") && !j["LINK_FROM"].is_null()) {
    //    j.at("LINK_FROM").get_to(t.tags);
    //}
    //else { t.link_from = {}; }
}
