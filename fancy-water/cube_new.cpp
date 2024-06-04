#include "cube_new.h"


cube_new::cube_new() : cube_new(1.0, bardrix::point3(0, 0, 0)) {}

cube_new::cube_new(double radius) : cube_new(radius, bardrix::point3(0, 0, 0)) {}

cube_new::cube_new(double radius, const bardrix::point3& position) : cube_new(radius, position, bardrix::material()) {}

cube_new::cube_new(double radius, const bardrix::point3& position, const bardrix::material& material) : radius_(radius), position_(position), material_(material) {}

void cube_new::set_material(const bardrix::material& material) { this->material_ = material; }

const bardrix::material& cube_new::get_material() const { return material_; }

void cube_new::set_position(const bardrix::point3& position) { this->position_ = position; }

const bardrix::point3& cube_new::get_position() const { return position_; }

bardrix::vector3 cube_new::normal_at(const bardrix::point3& intersection) const {
    return position_.vector_to(intersection).normalized();
}

std::optional<bardrix::point3> cube_new::intersection(const bardrix::ray& ray) const {
    // Get direction of the ray
    bardrix::vector3 direction = ray.get_direction();

    // Gets vector from points: ray origin and sphere center
    bardrix::vector3 ray_to_sphere_vector = ray.position.vector_to(position_);

    // Get dot product of origin-center-vector and normalized direction
    const double dot = ray_to_sphere_vector.dot(direction);

    // Turn unit vector direction into a vector direction
    direction *= dot;

    // Length from ray origin to sphere center
    ray_to_sphere_vector = direction - ray_to_sphere_vector;

    // vec.dot(vec) == |vec|^2
    const double distance_squared = ray_to_sphere_vector.dot(ray_to_sphere_vector);

    // Radius^2
    const double radius_squared = radius_ * radius_;

    if (distance_squared > radius_squared)
        return std::nullopt; // A smart way to check if ray intersects before taking the sqrt

    // Calculate distance to intersection
    const double distance = dot - std::sqrt(radius_squared - distance_squared);

    // If we intersect sphere return the length
    return (distance < ray.get_length() && distance > 0)
           ? std::optional(ray.position + ray.get_direction() * distance)
           : std::nullopt;
}