#include "commons.h"



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


glm::vec3 hex_to_rgb(std::string hex) {

    //Delete the first # char if there is one
    if (hex.at(0) == '#') {
        hex.erase(0, 1);
    }

    //Check if the hex correspond to a color
    if (hex.size() != 6) {
        SSS::log_err("Hex value is invalid");
        return glm::vec3{ 1.0f };
    }

    std::vector<std::string> colors = split_str(hex, 2);

    float r = static_cast<float>(std::stoi(colors[0], NULL, 16)) / 255.0f;
    float g = static_cast<float>(std::stoi(colors[1], NULL, 16)) / 255.0f;
    float b = static_cast<float>(std::stoi(colors[2], NULL, 16)) / 255.0f;

    return glm::vec3(r, g, b);
};



std::string rgb_to_hex(glm::vec3 rgb, bool head) {
    int r, g, b;

    r = std::floor(rgb.r * 255.0f);
    g = std::floor(rgb.g * 255.0f);
    b = std::floor(rgb.b * 255.0f);

    char hexColor[8];
    std::snprintf(hexColor, sizeof hexColor, "#%02x%02x%02x", r, g, b);
      
    return SSS::toString(hexColor);
}