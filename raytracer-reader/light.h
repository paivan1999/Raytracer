#pragma once

#include <vector.h>
#include <triangle.h>

struct Light {
    const Vector position;
    const Vector intensity;

    Light(const Vector& position, const Vector& intensity)
        : position(std::move(position)), intensity(std::move(intensity)) {
    }
};
