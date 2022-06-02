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

glm::vec3 hsl_to_rgb(glm::vec3 hsl)
{
    //int res = std::floor(hsl.x / 20);
    //switch (res) {
    //case 1 : 
    //}
    ////TODO
    return hsl;
}

glm::vec3 rgb_to_hsl(glm::vec3 rgb)
{
    float h, s, l, cmax, cmin, delta;
    
    cmax = std::max({ rgb.r, rgb.b, rgb.g });
    cmin = std::min({ rgb.r, rgb.b, rgb.g });

    delta = cmax - cmin;

    //LIGHTNESS
    l = (0.5 * cmax + cmin);

    //HUE 
    if (delta == 0) {
        return { 0,0,l };
    }


    if (cmax == rgb.r) {
        h = 60 * std::fmod(((rgb.g - rgb.b) / delta), 6.0);
    }

    if (cmax == rgb.g) {
        h = 60 * (((rgb.b - rgb.r) / delta) + 2.0);
    } 

    if (cmax == rgb.b) {
        h = 60 * (((rgb.r - rgb.g) / delta) + 2.0);
    }

    //SATURATION
    s = 1 / (1 - std::abs(2 * l - 1));


    std::cout << "{" << h << ", " << s << ", " << l << "}" << std::endl;
    return glm::vec3{ h, s, l };
}

float rand_float()
{
    return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX + 1);
}

std::string rand_color()
{
    return rgb_to_hex(glm::vec3(rand_float(), rand_float(), rand_float()));
}

