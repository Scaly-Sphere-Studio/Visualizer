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

float rand_float();
std::string rand_color();
glm::vec4 rand_pastel_color();