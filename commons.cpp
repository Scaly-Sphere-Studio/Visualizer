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


glm::vec4 hex_to_rgb(std::string hex) {

    //Delete the first # char if there is one
    if (hex.at(0) == '#') {
        hex.erase(0, 1);
    }

    //Check if the hex correspond to a color
    if (hex.size() != 6) {
        SSS::log_err("Hex value is invalid");
        return glm::vec4{ 1.0f };
    }

    std::vector<std::string> colors = split_str(hex, 2);

    float r = static_cast<float>(std::stoi(colors[0], NULL, 16)) / 255.0f;
    float g = static_cast<float>(std::stoi(colors[1], NULL, 16)) / 255.0f;
    float b = static_cast<float>(std::stoi(colors[2], NULL, 16)) / 255.0f;

    return glm::vec4(r, g, b, 1.0);
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

glm::vec4 hsl_to_rgb(glm::vec3 hsl)
{
    const float C = hsl.y * (1 - abs(2*hsl.z - 1));
    const float X = C * (1 - abs(fmod(hsl.x / 60.0, 2.0) - 1));
    const float m = hsl.z - C / 2.0 ;
    
    const unsigned int res = std::floor(hsl.x / 60.0);

    switch (res) {
        case 0: return { C + m, X + m, m, 1.0 };
        case 1: return { X + m ,C + m, m, 1.0 };
        case 2: return { m, C + m, X + m, 1.0 };
        case 3: return { m, X + m, C + m, 1.0 };
        case 4: return { X + m, m, C + m, 1.0 };
        case 5: return { C + m, m, X + m, 1.0 };
    }

    return { 0,0,0,0 };
}

glm::vec4 rgb_to_hsl(glm::vec3 rgb)
{
    const float cmax = std::max({ rgb.r, rgb.b, rgb.g });
    const float cmin = std::min({ rgb.r, rgb.b, rgb.g });
    const float delta = cmax - cmin;

    float h = 0.0f, s, l;

    //LIGHTNESS
    l = (cmax + cmin)/2.0f;

    //SATURATION
    s = delta / (1 - std::abs(2 * l - 1));

    //HUE 
    if (delta == 0) {
        return { 0,0,l, 1.0 };
    }

    if (cmax == rgb.r) {
        h = 60 * std::fmod(((rgb.g - rgb.b) / delta), 6.0);
    }

    if (cmax == rgb.g) {
        h = 60 * (((rgb.b - rgb.r) / delta) + 2.0);
    } 

    if (cmax == rgb.b) {
        h = 60 * (((rgb.r - rgb.g) / delta) + 4.0);
    }

    return glm::vec4{ h, s, l, 1.0 };
}

std::string rand_string() {
    return std::to_string(std::rand());
}

float rand_float()
{
    return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX + 1);
}

std::string rand_color()
{
    return rgb_to_hex(glm::vec3(rand_float(), rand_float(), rand_float()));
}

