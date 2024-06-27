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
    bardrix::vector3 dir_inv = { 1.0f / ray.get_direction().x, 1.0f / ray.get_direction().y, 1.0f / ray.get_direction().z };//Inverted ray direction
    float tmin = 0.0f, tmax = std::numeric_limits<float>::infinity();

    //Get longest vertex (diagonal-based)
    bardrix::point3 corner(dimention.x * 0.5, dimention.y * 0.5, dimention.z * 0.5);
    bardrix::point3 min_corner = position_ - corner;
    bardrix::point3 max_corner = position_ + corner;
    //

    //Calc nearest and farthest point
    double origin[3] = { ray.position.x, ray.position.y, ray.position.z };
    double min[3] = { min_corner.x, min_corner.y, min_corner.z };
    double max[3] = { max_corner.x, max_corner.y, max_corner.z };
    //

    for (int d = 0; d < 3; ++d) {//Quick dot-product based check for early intersection
        float t1 = (min[d] - origin[d]) * (d == 0 ? dir_inv.x : (d == 1 ? dir_inv.y : dir_inv.z));
        float t2 = (max[d] - origin[d]) * (d == 0 ? dir_inv.x : (d == 1 ? dir_inv.y : dir_inv.z));

        if (t1 > t2) std::swap(t1, t2);

        tmin = std::max(tmin, t1);
        tmax = std::min(tmax, t2);

        if (tmin > tmax) {//Dot product is negative
            return std::nullopt;
        }
    }

    //Final intersection check
    return (tmin < tmax && tmin < ray.get_length() && tmin > 0)
        ? std::optional<bardrix::point3>{{ray.position.x + ray.get_direction().x * tmin, ray.position.y + ray.get_direction().y * tmin, ray.position.z + ray.get_direction().z * tmin}}
        : std::nullopt;
}
