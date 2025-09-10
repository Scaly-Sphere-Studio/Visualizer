#pragma once

#include "_includes.hpp"
#include <random>

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
SSS::RGBA_f rand_color();
SSS::RGBA_f rand_pastel_color();