#pragma once

#include <vector.h>

class Intersection {
public:
    Intersection(Vector pos, Vector norm, double dist)
        : position_(pos), normal_(norm), distance_(dist) {
    }

    const Vector& GetPosition() const {
        return position_;
    };
    const Vector& GetNormal() const {
        return normal_;
    };
    double GetDistance() const {
        return distance_;
    };

private:
    Vector position_;
    Vector normal_;
    double distance_;
};
