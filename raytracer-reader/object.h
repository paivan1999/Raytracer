#pragma once

#include <triangle.h>
#include <material.h>
#include <sphere.h>
#include <geometry.h>

struct Object {
    const Material *material = nullptr;
    const Triangle polygon{};
    const std::array<Vector, 3> normals;
    Object() {
    }
    Object(Material *material, Triangle polygon, std::array<Vector, 3> normals)
        : material(material), polygon(polygon), normals(normals) {
    }
    Vector GetNormalAtPoint(Vector p) const {
        Vector bar = GetBarycentricCoords(polygon, p);
        return normals[0] * bar[0] + normals[1] * bar[1] + normals[2] * bar[2];
    }
    const Vector *GetNormal(size_t index) const {
        return &normals[index];
    };
};

struct SphereObject {
    const Material *material = nullptr;
    Sphere sphere{};
    SphereObject() {
    }
    SphereObject(Material *material, Sphere sphere) : material(material), sphere(sphere) {
    }
    Vector GetNormalAtPoint(Vector p) const {
        Vector normal = p - sphere.GetCenter();
        normal.Normalize();
        return normal;
    }
};

struct FinalObject {
public:
    Object object{};
    SphereObject sphere_object{};
    FinalObject(SphereObject sphere_object) : sphere_object(sphere_object) {
    }
    FinalObject(Object object) : object(object), if_triangle_(true) {
    }
    bool IfTriangle() const {
        return if_triangle_;
    }

private:
    bool if_triangle_ = false;
};
