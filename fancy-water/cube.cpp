#include "cube.h"
#include <optional>
#include <algorithm>
#include <bardrix/objects.h>

cube::cube() : cube(bardrix::point3(1.0, 1.0, 1.0), bardrix::point3(0, 0, 0)) {}

cube::cube(const bardrix::point3& dimention) : cube(dimention, bardrix::point3(0, 0, 0)) {}

cube::cube(const bardrix::point3& dimention, const bardrix::point3& position) : cube(dimention, position, bardrix::material()) {}

cube::cube(const bardrix::point3& dimention, const bardrix::point3& position, const bardrix::material& material) : dimention(dimention), position_(position), material_(material) {}

void cube::set_material(const bardrix::material& material) { this->material_ = material; }

const bardrix::material& cube::get_material() const { return material_; }

void cube::set_position(const bardrix::point3& position) { this->position_ = position; }

const bardrix::point3& cube::get_position() const { return position_; }

bardrix::vector3 cube::normal_at(const bardrix::point3& intersection) const {
    return position_.vector_to(intersection).normalized();
}

std::optional<bardrix::point3> cube::intersection(const bardrix::ray& ray) const {
    const bardrix::vector3& direction = ray.get_direction();
    bardrix::vector3 dir_inv = { 1.0f / direction.x, 1.0f / direction.y, 1.0f / direction.z };//Inverted direction of the ray
    float tmin = 0.0f, tmax = std::numeric_limits<float>::infinity();
    //Get longest line in cube. (Diagonal-based)
    bardrix::point3 min_corner = position_ - dimention * 0.5;
    bardrix::point3 max_corner = position_ + dimention * 0.5;
    //

    double origin[3] = { ray.position.x, ray.position.y, ray.position.z };
    //Calc nearest and farthest intersection
    double min[3] = { min_corner.x, min_corner.y, min_corner.z };
    double max[3] = { max_corner.x, max_corner.y, max_corner.z };
    //

    for (int d = 0; d < 3; ++d) {//Quick dot-product based check for intersection.
        float t1 = (min[d] - origin[d]) * (d == 0 ? dir_inv.x : (d == 1 ? dir_inv.y : dir_inv.z));
        float t2 = (max[d] - origin[d]) * (d == 0 ? dir_inv.x : (d == 1 ? dir_inv.y : dir_inv.z));

        if (t1 > t2) std::swap(t1, t2);

        tmin = std::max(tmin, t1);
        tmax = std::min(tmax, t2);

        if (tmin > tmax) {
            return std::nullopt;
        }
    }

    float t_distance = tmin;

    //Calc intersection point
    bardrix::point3 intersection_point = {
        ray.position.x + direction.x * t_distance,
        ray.position.y + direction.y * t_distance,
        ray.position.z + direction.z * t_distance
    };
    //
    return (t_distance < ray.get_length() && t_distance > 0) ? std::optional<bardrix::point3>{ intersection_point } : std::nullopt;
}
