#pragma once
#ifndef SHAD_TASK_DIR
#define SHAD_TASK_DIR "./"
#endif

#include <sstream>
#include <iostream>

#include <material.h>
#include <vector.h>
#include <object.h>
#include <light.h>
#include <reader.h>

#include <vector>
#include <map>
#include <string>
#include <fstream>

class Scene {
private:
    std::vector<Object> objects_{};
    std::vector<SphereObject> sphere_objects_{};
    std::vector<Light> lights_;
    std::map<std::string, Material> materials_;

public:
    //    Scene(const Scene& scene) :materials_(scene.materials_),lights_(scene.lights_){
    //        for (auto sphereobject : scene.sphere_objects_) {
    //
    //        }
    //    }
    Scene(std::map<std::string, Material> materials, std::vector<Light> lights,
          std::vector<SphereObject> sphere_objects, std::vector<Object> objects)
        : lights_(lights), materials_(materials) {
        for (auto sphere : sphere_objects) {
            sphere_objects_.push_back(
                SphereObject(&materials_[sphere.material->name], sphere.sphere));
        }
        for (auto object : objects) {
            objects_.push_back(
                Object(&materials_[object.material->name], object.polygon, object.normals));
        }
    }
    //    Scene(const Scene& other)
    //        : Scene(other.GetMaterials(), other.GetLights(), other.GetSphereObjects(),
    //                other.GetObjects()) {
    //    }
    //    Scene(Scene&& other){
    //        std::swap(*this,other);
    //    }
    //    Scene& operator= (const Scene& other) {
    //        Scene copy(other);
    //        std::swap(*this,copy);
    //        return *this;
    //    }
    //    Scene& operator=(Scene&& other){
    //        std::swap(*this,other);
    //        return *this;
    //    }
    //    ~Scene() = default;

    const std::vector<Object>& GetObjects() const {
        return objects_;
    };
    const std::vector<SphereObject>& GetSphereObjects() const {
        return sphere_objects_;
    };
    const std::vector<Light>& GetLights() const {
        return lights_;
    };
    const std::map<std::string, Material>& GetMaterials() const {
        return materials_;
    };
};

inline std::map<std::string, Material> ReadMaterials(std::string_view filename) {
    std::map<std::string, Material> result;
    std::ifstream infile;
    infile.open(filename.data());

    bool begun = false;
    Material current_material{};
    for (std::string line; std::getline(infile, line);) {
        ReaderMtl line_reader(line);
        if (line_reader.Newmtl()) {
            if (begun) {
                result.insert({current_material.name, current_material});
            }
            begun = true;
            current_material =
                Material{"", Vector(), Vector(), Vector(), Vector(), 0, 0, {1, 0, 0}};
            current_material.name = line_reader.GetNewmtl();
        }
        if (line_reader.Ka()) {
            current_material.ambient_color = line_reader.GetKaKdKsKe();
        }
        if (line_reader.Kd()) {
            current_material.diffuse_color = line_reader.GetKaKdKsKe();
        }
        if (line_reader.Ks()) {
            current_material.specular_color = line_reader.GetKaKdKsKe();
        }
        if (line_reader.Ke()) {
            current_material.intensity = line_reader.GetKaKdKsKe();
        }
        if (line_reader.Ns()) {
            current_material.specular_exponent = line_reader.GetNsNi();
        }
        if (line_reader.Ni()) {
            current_material.refraction_index = line_reader.GetNsNi();
        }
        if (line_reader.Al()) {
            current_material.albedo = line_reader.GetAl();
        }
    }
    result[current_material.name] = current_material;
    return result;
}
inline Scene ReadScene(const std::string& filename) {
    std::vector<Object> objects{};
    std::vector<SphereObject> sphere_objects{};
    std::vector<Light> lights{};
    std::map<std::string, Material> materials;
    std::string cut_filename =
        static_cast<std::string>(filename.substr(0, filename.find_last_of('/') + 1));
    std::ifstream infile;
    infile.open(filename.data());

    std::vector<Vector> vs{Vector()};
    std::vector<Vector> vns{Vector()};
    std::string current_material;
    for (std::string line; std::getline(infile, line);) {
        ReaderObj line_reader(line);
        if (line_reader.Mtllib()) {
            materials = ReadMaterials(cut_filename + line_reader.GetMtllibUsemtl());
        }
        if (line_reader.Usemtl()) {
            current_material = line_reader.GetMtllibUsemtl();
        }
        if (line_reader.V()) {
            vs.push_back(line_reader.GetVnV());
        }
        if (line_reader.Vn()) {
            vns.push_back(line_reader.GetVnV());
        }
        if (line_reader.P()) {
            Vector position;
            Vector intensity;
            std::tie(position, intensity) = line_reader.GetP();
            lights.push_back(Light(position, intensity));
        }
        if (line_reader.S()) {
            Vector center;
            double radius;
            std::tie(center, radius) = line_reader.GetS();
            sphere_objects.push_back(
                SphereObject(&materials[current_material], Sphere(center, radius)));
        }
        if (line_reader.F()) {
            auto v = line_reader.GetF();
            std::vector<Triangle> triangles{};
            std::vector<std::array<Vector, 3>> normals{};
            int l = v.size();
            int ind_v0;
            std::optional<int> ind_vn0;
            std::tie(ind_v0, ind_vn0) = v[0];
            int vss = vs.size();
            int vnss = vns.size();
            Vector v0 = vs[(vss + ind_v0) % vss];

            for (int i = 1; i < l - 1; ++i) {
                int ind_v1, ind_v2;
                std::optional<int> ind_vn1, ind_vn2;
                std::tie(ind_v1, ind_vn1) = v[i];
                std::tie(ind_v2, ind_vn2) = v[i + 1];
                Triangle triangle({v0, vs[(vss + ind_v1) % vss], vs[(vss + ind_v2) % vss]});
                triangles.push_back(triangle);
                if (ind_vn0.has_value() && ind_vn1.has_value() && ind_vn2.has_value()) {
                    normals.push_back({vns[(vnss + ind_vn0.value()) % vnss],
                                       vns[(vnss + ind_vn1.value()) % vnss],
                                       vns[(vnss + ind_vn2.value()) % vnss]});
                } else {
                    Vector normal = triangle.GetNormal();
                    normals.push_back({normal, normal, normal});
                }
            }
            for (int i = 0; i < l - 2; ++i) {
                objects.push_back(Object(&materials[current_material], triangles[i], normals[i]));
            }
        }
    }
    return Scene(materials, lights, sphere_objects, objects);
}
