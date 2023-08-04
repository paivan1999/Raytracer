#pragma once
#include <ray.h>

struct Pixel {
    Pixel(const Ray direction, double distance, int x, int y)
        : direction(direction), distance(distance), x(x), y(y) {
    }
    const Ray direction;
    const double distance;
    const int x, y;
    Vector color{0, 0, 0};
    RGB rgb{0, 0, 0};
};
