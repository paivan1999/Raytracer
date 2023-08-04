#include <vector.h>
#include <sphere.h>
#include <intersection.h>
#include <triangle.h>
#include <ray.h>

#include <optional>
const double kMykErr = 1e-8;
bool RootExists(double a, double b, double c) {
    double d = b * b - 4 * a * c;
    return d >= 0;
}
std::pair<double, double> Root(double a, double b, double c) {
    double d = b * b - 4 * a * c;
    return std::make_pair<double, double>((-b - sqrt(d)) / 2 / a, (-b + sqrt(d)) / 2 / a);
}
bool OneSide(Vector v1, Vector v2) {
    return DotProduct(v1, v2) > 0;
}
std::optional<Intersection> GetIntersection(const Ray& ray, const Sphere& sphere) {
    const Vector &start = ray.GetOrigin(), direction = ray.GetDirection();
    const Vector& center = sphere.GetCenter();
    double r = sphere.GetRadius();
    const Vector delta = start - center;
    double a = DotProduct(direction, direction);
    double b = 2 * DotProduct(direction, delta);
    double c = DotProduct(delta, delta) - r * r;
    if (!RootExists(a, b, c)) {
        return {};
    }
    const std::pair<double, double> pair = Root(a, b, c);
    double r1 = pair.first, r2 = pair.second;
    if (r2 < 0) {
        return {};
    }
    double pk;
    if (r1 >= 0) {
        pk = r1;
    } else {
        pk = r2;
    }
    const Vector p = start + (direction * pk);
    Vector norm = p - center;
    norm.Normalize();
    if (OneSide(norm, direction)) {
        norm = norm * (-1);
    }
    return Intersection{p, norm, Length(direction) * pk};
}

std::optional<Intersection> GetIntersection(const Ray& ray, const Triangle& triangle) {
    const Vector& vertex0 = triangle.GetVertex(0);
    const Vector& vertex1 = triangle.GetVertex(1);
    const Vector& vertex2 = triangle.GetVertex(2);
    Vector edge1, edge2, h, s, q;
    double a, f, u, v;
    edge1 = vertex1 - vertex0;
    edge2 = vertex2 - vertex0;
    h = CrossProduct(ray.GetDirection(), edge2);
    a = DotProduct(edge1, h);
    if (a > -kMykErr && a < kMykErr) {
        return {};  // This ray is parallel to this triangle.
    }
    f = 1.0 / a;
    s = ray.GetOrigin() - vertex0;
    u = f * DotProduct(s, h);
    if (u < 0.0 || u > 1.0) {
        return {};
    }
    q = CrossProduct(s, edge1);
    v = f * DotProduct(ray.GetDirection(), q);
    if (v < 0.0 || u + v > 1.0) {
        return {};
    }
    // At this stage we can compute t to find out where the intersection point is on the line.
    double t = f * DotProduct(edge2, q);
    if (t > kMykErr)  // ray intersection
    {
        Vector p = ray.GetOrigin() + ray.GetDirection() * t;
        double dist = Length(ray.GetDirection()) * t;
        Vector suspect_norm = CrossProduct(edge1, edge2);
        suspect_norm.Normalize();
        if (OneSide(suspect_norm, ray.GetDirection())) {
            suspect_norm = suspect_norm * (-1);
        }
        return Intersection{p, suspect_norm, dist};
    } else {  // This means that there is a line intersection but not a ray intersection.
        return {};
    }
}

std::optional<Vector> Refract(const Vector& ray, const Vector& normal, double eta) {
    Vector my_ray = ray;
    my_ray.Normalize();
    Vector my_normal = normal;
    my_normal.Normalize();
    double my_cos = -DotProduct(my_ray, normal);
    double my_cos_square = my_cos * my_cos;
    double my_sin_square = 1 - my_cos_square;
    double eta_square = eta * eta;
    if (my_sin_square >= 1 / eta_square) {
        return {};
    } else {
        return my_ray * eta +
               my_normal * (eta * my_cos - sqrt(1 - eta_square * (1 - my_cos_square)));
    }
}
Vector Reflect(const Vector& ray, const Vector& normal) {
    double c = -DotProduct(ray, normal);
    Vector result = ray + normal * (2 * c);
    result.Normalize();
    return result;
}
inline double Determinant(const Vector& v1, const Vector& v2, int ind1, int ind2) {
    return v1[ind1] * v2[ind2] - v1[ind2] * v2[ind1];
}
inline std::pair<double, double> Solve(const Vector& v1, const Vector& v2, const Vector& v3,
                                       int ind1, int ind2, double det) {
    double x1 = v1[ind1], x2 = v2[ind1], x3 = v3[ind1], y1 = v1[ind2], y2 = v2[ind2], y3 = v3[ind2];
    double answer1 = (y2 * x3 - x2 * y3) / det, answer2 = (x1 * y3 - x3 * y1) / det;
    return std::make_pair(answer1, answer2);
}
Vector GetBarycentricCoords(const Triangle& triangle, const Vector& point) {
    const Vector &a = triangle.GetVertex(0), b = triangle.GetVertex(1), c = triangle.GetVertex(2);
    Vector p = point - a, ab = b - a, ac = c - a;
    std::array<std::pair<int, int>, 3> indexes = {std::make_pair(0, 1), std::make_pair(0, 2),
                                                  std::make_pair(1, 2)};
    for (auto [ind1, ind2] : indexes) {
        double det = Determinant(ab, ac, ind1, ind2);
        if (det < -kMykErr || det > kMykErr) {
            auto pair = Solve(ab, ac, p, ind1, ind2, det);
            double beta = pair.first, gamma = pair.second;
            double alpha = 1 - beta - gamma;
            return {alpha, beta, gamma};
        }
    }
    return {0, 0, 0};
}
