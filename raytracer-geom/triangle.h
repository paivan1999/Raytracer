#pragma once

#include <initializer_list>
#include <vector.h>
#include <cmath>

class Triangle {
public:
    //    Triangle() : vertices_({Vector{0,0,0},Vector{0,0,0},Vector{0,0,0}}){
    //    }
    Triangle() {
    }
    Triangle(std::initializer_list<Vector> list) {
        auto d = data(list);
        vertices_ = {d[0], d[1], d[2]};
    }
    Vector GetNormal() const {
        Vector a = vertices_[2] - vertices_[0];
        Vector b = vertices_[1] - vertices_[0];
        Vector normal = CrossProduct(a, b);
        normal.Normalize();
        return normal;
    }

    double Area() const {
        Vector a = vertices_[2] - vertices_[0];
        Vector b = vertices_[1] - vertices_[0];
        return fabs(Length(CrossProduct(a, b))) / 2;
    }

    const Vector& GetVertex(size_t ind) const {
        return vertices_[ind];
    }


private:
    std::array<Vector, 3> vertices_ = {Vector(), Vector(), Vector()};
};
