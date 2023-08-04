#pragma once

#include <image.h>
#include <camera_options.h>
#include <render_options.h>
#include <string>
#include <scene.h>
#include <view.h>
#include <image.h>
#include <float.h>
#include <cmath>
#include <color_transformation.h>
#include <algorithm>

const double kEps = 1e-5;
struct Closest {
    Closest(const FinalObject& obj, double dist) : final_object(obj), distance(dist) {
    }
    const FinalObject& final_object;
    double distance;
};
Vector Point(const Closest& closest, const Ray& ray) {
    Vector direction = ray.GetDirection();
    direction.Normalize();
    return ray.GetOrigin() + direction * closest.distance;
}

Vector Convert(const Vector& to_p) {
    Vector result = to_p;
    result.Normalize();
    return result * (-1);
}
Vector ToCorrectNormal(const FinalObject& obj, const Vector& point,
                       const Vector& initial_ray_direction) {

    Vector normal = obj.IfTriangle() ? obj.object.GetNormalAtPoint(point)
                                     : obj.sphere_object.GetNormalAtPoint(point);
    //    if (obj.IfTriangle()) {
    //        normal = obj.object.GetNormalAtPoint(point);
    //    } else {
    //        normal = obj.sphere_object.GetNormalAtPoint(point);
    //    }
    if (OneSide(initial_ray_direction, normal)) {
        normal = normal * (-1);
    }
    return normal;
}

std::optional<Closest> GetClosest(const std::vector<FinalObject>& objects, const Ray& ray) {
    double current_distance = DBL_MAX;
    std::optional<Closest> current{};
    for (const auto& obj : objects) {
        std::optional<Intersection> intersection =
            obj.IfTriangle() ? GetIntersection(ray, obj.object.polygon)
                             : GetIntersection(ray, obj.sphere_object.sphere);

        //        if (obj.IfTriangle()) {
        //            intersection = GetIntersection(ray, obj.object.polygon);
        //        } else {
        //            intersection = GetIntersection(ray, obj.sphere_object.sphere);
        //        }
        if (intersection.has_value()) {
            double dist = intersection.value().GetDistance();
            if (dist < current_distance) {
                current_distance = dist;
                current.emplace(Closest(obj, current_distance));
            }
        }
    }
    return current;
}

Image ImageFromPixels(std::vector<Pixel>* pixels, const CameraOptions& camera_options,
                      RenderMode mode) {
    if (mode == RenderMode::kFull) {
        FullToRGB(pixels);
    }
    if (mode == RenderMode::kDepth) {
        DepthToRGB(pixels);
    }
    if (mode == RenderMode::kNormal) {
        NormalToRGB(pixels);
    }
    Image image(camera_options.screen_width, camera_options.screen_height);
    for (const Pixel& pixel : *pixels) {
        int x = pixel.x, y = pixel.y;
        image.SetPixel(pixel.rgb, y, x);
    }
    return image;
}

std::vector<FinalObject> GetFinalObjects(const std::vector<Object>& triangles,
                                         const std::vector<SphereObject>& spheres) {
    std::vector<FinalObject> objects{};
    for (const auto& obj : triangles) {
        objects.push_back(FinalObject(obj));
    }
    for (const auto& sphere : spheres) {
        objects.push_back(FinalObject(sphere));
    }
    return objects;
}

bool CheckIfLightedByOneLight(const Ray& ray, const std::vector<FinalObject>& objects) {
    //    const Vector& from = ray.GetOrigin();
    const Vector& to_p = ray.GetDirection();
    double max = Length(to_p);
    const std::optional<Closest> closest = GetClosest(objects, ray);
    if (!closest.has_value()) {
        return false;
    }
    //    assert(closest.has_value());
    return closest.value().distance >= max - kMykErr;
}
Vector DiffuseByOneLight(const std::vector<FinalObject>& objects, const Light& light,
                         const FinalObject& obj, const Vector& p) {
    const Vector& from = light.position;
    const Vector to_p = p - from;
    const Vector converted_to_p = Convert(to_p);
    //    const std::optional<Closest> closest = GetClosest(objects, Ray(from,to_p));
    if (!CheckIfLightedByOneLight(Ray(from, to_p), objects)) {
        return {0, 0, 0};
    }
    const Vector normal = ToCorrectNormal(obj, p, to_p);
    return light.intensity ^ (obj.IfTriangle() ? obj.object.material->diffuse_color
                                               : obj.sphere_object.material->diffuse_color) *
                                 DotProduct(normal, converted_to_p);
}
Vector SpecularByOneLight(const std::vector<FinalObject>& objects, const Light& light,
                          const Vector& initial_ray_direction, const FinalObject& obj,
                          const Vector& p) {
    const Vector& from = light.position;
    const Vector to_p = p - from;
    const Vector converted = Convert(initial_ray_direction);
    //    const std::optional<Closest> closest = GetClosest(objects, Ray(from,to_p));
    if (!CheckIfLightedByOneLight(Ray(from, to_p), objects)) {
        return {0, 0, 0};
    }
    const Vector reflected = Reflect(to_p, ToCorrectNormal(obj, p, to_p));
    return light.intensity ^
           (obj.IfTriangle() ? obj.object.material->specular_color
                             : obj.sphere_object.material->specular_color) *
               pow(std::max(0.0, DotProduct(converted, reflected)),
                   obj.IfTriangle() ? obj.object.material->specular_exponent
                                    : obj.sphere_object.material->specular_exponent);
}

Vector GetBaseColor(const std::vector<FinalObject>& objects, const std::vector<Light>& lights,
                    const Vector& initial_ray_direction, const FinalObject& obj, const Vector& p) {
    Vector result;
    for (const Light& light : lights) {
        const Vector& from = light.position;
        const Vector to_p = p - from;
        if (DotProduct(ToCorrectNormal(obj, p, to_p), initial_ray_direction) < 0) {
            result += SpecularByOneLight(objects, light, initial_ray_direction, obj, p);
            result += DiffuseByOneLight(objects, light, obj, p);
        }
    }
    result *=
        obj.IfTriangle() ? obj.object.material->albedo[0] : obj.sphere_object.material->albedo[0];
    result +=
        obj.IfTriangle() ? obj.object.material->intensity : obj.sphere_object.material->intensity;
    result += obj.IfTriangle() ? obj.object.material->ambient_color
                               : obj.sphere_object.material->ambient_color;
    return result;
}
Vector GetColor(const std::vector<FinalObject>& objects, const std::vector<Light>& lights,
                const Ray& initial_ray, int k, bool in) {
    Vector result;
    const std::optional<Closest> closest = GetClosest(objects, initial_ray);
    if (!closest.has_value()) {
        return {0, 0, 0};
    }
    const Vector p = Point(closest.value(), initial_ray);
    const FinalObject& obj = closest.value().final_object;
    const Vector& direction = initial_ray.GetDirection();
    result += GetBaseColor(objects, lights, direction, obj, p);
    if (k == 0) {
        return result;
    }
    const Vector normal = ToCorrectNormal(obj, p, direction);
    if (!in) {
        const Ray reflected(p + normal * kEps, Reflect(direction, normal));
        result += GetColor(objects, lights, reflected, k - 1, in) *
                  (obj.IfTriangle() ? obj.object.material->albedo[1]
                                    : obj.sphere_object.material->albedo[1]);
    }
    double eta = obj.IfTriangle() ? obj.object.material->refraction_index
                                  : obj.sphere_object.material->refraction_index;
    const std::optional<Vector> refracted = Refract(direction, normal, (!in) ? 1 / eta : eta);
    if (refracted.has_value()) {
        Ray ray(p - normal * kEps, refracted.value());
        if (!obj.IfTriangle()) {
            result += GetColor(objects, lights, ray, k - 1, !in) *
                      (in ? 1 : obj.sphere_object.material->albedo[2]);
        } else {
            result += GetColor(objects, lights, ray, k - 1, in) * obj.object.material->albedo[2];
        }
    }
    return result;
}
Image RenderFull(const std::string& filename, const CameraOptions& camera_options, int k) {
    std::vector<Pixel> pixels = GetView(camera_options);
    const Scene scene = ReadScene(filename);
    const auto objects = GetFinalObjects(scene.GetObjects(), scene.GetSphereObjects());
    const std::vector<Light>& lights = scene.GetLights();

    for (Pixel& pixel : pixels) {
        pixel.color = GetColor(objects, lights, pixel.direction, k, false);
    }
    return ImageFromPixels(&pixels, camera_options, RenderMode::kFull);
}
// const Pixel& search(int x, int y, const std::vector<Pixel>& pixels) {
//     for (const Pixel& pixel : pixels) {
//         if (pixel.x == x && pixel.y == y) {
//             return pixel;
//         }
//     }
// }
Image RenderDepth(const std::string& filename, const CameraOptions& camera_options) {
    std::vector<Pixel> pixels = GetView(camera_options);
    //    const Pixel& pixel = search(320,240,pixels);
    const Scene scene = ReadScene(filename);
    const auto objects = GetFinalObjects(scene.GetObjects(), scene.GetSphereObjects());
    for (Pixel& pixel : pixels) {
        std::optional<Closest> closest = GetClosest(objects, pixel.direction);
        if (closest.has_value()) {
            double dist = closest.value().distance;
            pixel.color = {dist, dist, dist};
        } else {
            pixel.color = {-1, -1, -1};
        }
    }
    return ImageFromPixels(&pixels, camera_options, RenderMode::kDepth);
}

Image RenderNormal(const std::string& filename, const CameraOptions& camera_options) {
    std::vector<Pixel> pixels = GetView(camera_options);
    const Scene scene = ReadScene(filename);
    const auto objects = GetFinalObjects(scene.GetObjects(), scene.GetSphereObjects());
    for (Pixel& pixel : pixels) {
        std::optional<Closest> closest = GetClosest(objects, pixel.direction);
        if (closest.has_value()) {
            FinalObject obj = closest.value().final_object;
            pixel.color = ToCorrectNormal(obj, Point(closest.value(), pixel.direction),
                                          pixel.direction.GetDirection());
        } else {
            pixel.color = {-1, -1, -1};
        }
    }
    return ImageFromPixels(&pixels, camera_options, RenderMode::kNormal);
}

Image Render(const std::string& filename, const CameraOptions& camera_options,
             const RenderOptions& render_options) {
//<<<<<<< HEAD
    if (render_options.mode == RenderMode::kFull) {
        return RenderFull(filename, camera_options, render_options.depth);
    } else if (render_options.mode == RenderMode::kDepth) {
        return RenderDepth(filename, camera_options);
    } else {
        return RenderNormal(filename, camera_options);
    }
}
//=======
//    throw std::runtime_error("Not implemented");
//}
//>>>>>>> 316e79ae7dc54b226704b508da5366faab504fba
