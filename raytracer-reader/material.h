#pragma once

#include <vector.h>
#include <string>

struct Material {
    std::string name;
    Vector ambient_color;
    Vector diffuse_color;
    Vector specular_color;
    Vector intensity;
    double specular_exponent;
    double refraction_index;
    std::array<double, 3> albedo;
};
