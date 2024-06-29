#include "cube.h"
#include "point2.hpp"
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
    bardrix::point3 corner = dimention * 0.5;
    bardrix::point3 min_corner = position_ - corner;
    bardrix::point3 max_corner = position_ + corner;

    double dx_min = std::abs(intersection.x - min_corner.x);
    double dx_max = std::abs(intersection.x - max_corner.x);
    double dy_min = std::abs(intersection.y - min_corner.y);
    double dy_max = std::abs(intersection.y - max_corner.y);
    double dz_min = std::abs(intersection.z - min_corner.z);
    double dz_max = std::abs(intersection.z - max_corner.z);

    double min_distance = std::min({ dx_min, dx_max, dy_min, dy_max, dz_min, dz_max });

    if (min_distance == dx_min) return bardrix::vector3(-1, 0, 0);   // Left face
    if (min_distance == dx_max) return bardrix::vector3(1, 0, 0);    // Right face
    if (min_distance == dy_min) return bardrix::vector3(0, -1, 0);   // Bottom face
    if (min_distance == dy_max) return bardrix::vector3(0, 1, 0);    // Top face
    if (min_distance == dz_min) return bardrix::vector3(0, 0, -1);   // Back face
    if (min_distance == dz_max) return bardrix::vector3(0, 0, 1);    // Front face

    return bardrix::vector3(0, 0, 0);  // Default case (should never happen for a well-formed cube)
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

point2 cube::get_texture_coordinates(const bardrix::point3& intersection) const {
    bardrix::vector3 normal = normal_at(intersection);
    point2 uv;

    if (normal.x == 1 || normal.x == -1) {
        uv.x = (intersection.z - (position_.z - dimention.z * 0.5)) / dimention.z;
        uv.y = (intersection.y - (position_.y - dimention.y * 0.5)) / dimention.y;
    }
    else if (normal.y == 1 || normal.y == -1) {
        uv.x = (intersection.x - (position_.x - dimention.x * 0.5)) / dimention.x;
        uv.y = (intersection.z - (position_.z - dimention.z * 0.5)) / dimention.z;
    }
    else if (normal.z == 1 || normal.z == -1) {
        uv.x = (intersection.x - (position_.x - dimention.x * 0.5)) / dimention.x;
        uv.y = (intersection.y - (position_.y - dimention.y * 0.5)) / dimention.y;
    }

    return uv;
}