#pragma once

#include <string>
#include <SSS/GL.hpp>


//Split a string into multiple sub string with a fixed length
std::vector<std::string> split_str(std::string src, unsigned int split_len);

//Fill a vec3 color from its hex value
glm::vec3 hex_to_rgb(std::string hex);
