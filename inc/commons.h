#pragma once

#include "_includes.hpp"

// Converts a value to a hex string by outputing it to a string stream
template <typename T>
std::string hex_convertor(T const& arg) noexcept try
{
    std::ostringstream strstream;
    strstream << std::hex << arg;
    return strstream.str();
}
catch (...) {
    return "[SSS::toHex() error]";
}

//Split a string into multiple sub string with a fixed length
std::vector<std::string> split_str(std::string src, unsigned int split_len);

////Fill a vec3 color from its hex value
//glm::vec4 hex_to_rgb(std::string hex);
////Retrieve the hex value from its composants
//std::string rgb_to_hex(glm::vec3 rgb, bool head = 1);
//glm::vec4 hsl_to_rgb(glm::vec3 hsl);
//glm::vec4 rgb_to_hsl(glm::vec3 rgb);
//uint32_t rgb_to_int32t(glm::vec4 rgb);

float rand_float();
std::string rand_color();
glm::vec4 rand_pastel_color();