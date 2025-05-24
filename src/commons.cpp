#include "commons.h"
#include "SSS/Commons/color.hpp"


std::vector<std::string> split_str(std::string src, unsigned int split_len) {

    unsigned int nb_split = static_cast<unsigned int>(std::floor(src.size() / split_len));
    std::vector<std::string> split_vec;

    for (unsigned int i = 0; i < nb_split; i++) {
        split_vec.push_back(src.substr(i * split_len, split_len));
    }

    if (src.size() % split_len != 0) {
        split_vec.push_back(src.substr(nb_split * split_len));
    }

    return split_vec;
};


std::string rand_string() {
    return std::to_string(std::rand());
}

float rand_float()
{
    return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX + 1.f);
}

std::string rand_color()
{
    SSS::RGBA_f c(glm::vec4(rand_float(), rand_float(), rand_float(), 1.0));
    return c.to_Hex();
}

glm::vec4 rand_pastel_color()
{

    SSS::RGBA_f c;
    c.from_HSL(glm::vec4(360.f * rand_float(),
        0.25f + 0.7f * rand_float(),
        0.85f + 0.1f * rand_float(), 1.0));
    return c.to_RGBA();
}
