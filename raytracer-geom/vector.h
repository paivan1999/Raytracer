#pragma once

#include <array>
#include <cmath>
#include <iostream>
#include <initializer_list>
#include <algorithm>

class Vector {
public:
    Vector() : data_({0, 0, 0}) {
    }
    Vector(std::initializer_list<double> list) {
        auto my_data = data(list);
        data_ = {my_data[0], my_data[1], my_data[2]};
    }
    Vector(std::array<double, 3> data) : data_(data) {
    }

    double& operator[](size_t ind) {
        return data_[ind];
    };
    double operator[](size_t ind) const {
        return data_[ind];
    };
    Vector operator+(const Vector& other) const {
        return {data_[0] + other[0], data_[1] + other[1], data_[2] + other[2]};
    }
    Vector operator+=(const Vector& other) {
        *this = *this + other;
        return *this;
    }
    Vector operator*=(double k) {
        *this = *this * k;
        return *this;
    }
    Vector operator-(const Vector& other) const {
        return {data_[0] - other[0], data_[1] - other[1], data_[2] - other[2]};
    }
    Vector operator*(double k) const {
        return {k * data_[0], k * data_[1], k * data_[2]};
    }
    Vector operator^(Vector other) const {
        return Vector{data_[0] * other[0], data_[1] * other[1], data_[2] * other[2]};
    }
    void Normalize() {
        double mod = sqrt(data_[0] * data_[0] + data_[1] * data_[1] + data_[2] * data_[2]);
        data_ = {data_[0] / mod, data_[1] / mod, data_[2] / mod};
    };

private:
    std::array<double, 3> data_;
};

inline double DotProduct(const Vector& lhs, const Vector& rhs) {
    return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2];
}
inline Vector CrossProduct(const Vector& a, const Vector& b) {
    return {a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]};
}
inline double Length(const Vector& vec) {
    return sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
}

inline Vector Multiply(const Vector& v, const double k) {
    return {v[0] * k, v[1] * k, v[2] * k};
}
