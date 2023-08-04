#pragma once

#include <vector.h>
#include <camera_options.h>
#include <pixel.h>

#include <cmath>
double Dx(const CameraOptions& camera_options, int x) {
    double angle_v = camera_options.fov / 2;
    int width_p = camera_options.screen_width;
    int height_p = camera_options.screen_height;
    double height = tan(angle_v) * 2;
    double width = height * width_p / height_p;
    double lx = -width / 2;
    return lx + (x + static_cast<double>(1) / 2) * width / width_p;
}
double Dy(const CameraOptions& camera_options, int y) {
    double angle_v = camera_options.fov / 2;
    double height_p = camera_options.screen_height;
    double height = tan(angle_v) * 2;
    double uy = height / 2;
    return uy - (y + static_cast<double>(1) / 2) * height / height_p;
}
Vector DirectionToPixel(double x, double y, double z, const Vector& x_v, const Vector& y_v,
                        const Vector& z_v) {
    return x_v * x + y_v * y + z_v * z;
}
bool CheckDoubleForError(double real, double target, double err) {
    double delta = real - target;
    return delta < err && delta > -err;
}
bool CheckVectorForError(const Vector& real, const Vector& target, double err) {
    return CheckDoubleForError(real[0], target[0], err) &&
           CheckDoubleForError(real[1], target[1], err) &&
           CheckDoubleForError(real[1], target[1], err);
}
std::vector<Pixel> GetView(const CameraOptions& camera_options) {
    std::vector<Pixel> result{};
    const double err = 1e-8;
    const Vector from = camera_options.look_from;
    const Vector to = camera_options.look_to;
    int width_p = camera_options.screen_width;
    int height_p = camera_options.screen_height;
    Vector z_v = from - to;
    z_v.Normalize();
    Vector up{0, 1, 0};
    Vector x_v;
    if (CheckVectorForError(z_v, up, err) || CheckVectorForError(z_v, up * (-1), err)) {
        x_v = {1, 0, 0};
    } else {
        x_v = CrossProduct(up, z_v);
        x_v.Normalize();
    }
    Vector y_v = CrossProduct(z_v, x_v);
    y_v.Normalize();
    for (int x = 0; x < width_p; ++x) {
        for (int y = 0; y < height_p; ++y) {
            Vector direction =
                DirectionToPixel(Dx(camera_options, x), Dy(camera_options, y), -1, x_v, y_v, z_v);
            double distance = Length(direction);
            direction.Normalize();
            result.push_back(
                Pixel(Ray(Vector(camera_options.look_from), direction), distance, x, y));
        }
    }
    return result;
}
