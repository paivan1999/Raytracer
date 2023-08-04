#pragma once

#include <vector.h>

class Sphere {
public:
    Sphere() {
    }
    Sphere(Vector center, double radius) : center_(center), radius_(radius) {
    }
    const Vector& GetCenter() const {
        return center_;
    };
    double GetRadius() const {
        return radius_;
    };

private:
    Vector center_{};
    double radius_ = 0;
};
