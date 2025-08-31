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
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<> dist(0.0f, 1.0f);

    return dist(rng);
}

std::string rand_color()
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<> dist(0.0f, 1.0f);

    return SSS::RGBA_f(glm::vec4(dist(rng), dist(rng), dist(rng), 1.0));
}

glm::vec4 rand_pastel_color()
{
    float H = 360.f * rand_float();
    float S = 0.25f + 0.7f * rand_float();
    float L = 0.85f + 0.1f * rand_float();

    return SSS::RGBA_f::from_HSL(glm::vec4(H, S, L, 1.0));
}
