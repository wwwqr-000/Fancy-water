//
// Created by Bardio on 22/05/2024.
//

#include <iostream>
#include <thread>
#include <vector>
#include <future>
#include <algorithm>

#ifdef _WIN32

#include "sphere.h"
#include "window.h"

#include <bardrix/ray.h>
#include <bardrix/light.h>
#include <bardrix/camera.h>
#include <bardrix/color.h>

//For delta tick function
#include <mutex>
#include <cstdint>
#include <chrono>
#include <condition_variable>
#include <bardrix/quaternion.h>
//

bool running = true;

std::vector<std::thread> threadVec;

void tickFunc(bardrix::light &light, bardrix::window &window) {
    while (running) {
        auto start = std::chrono::system_clock::now();
        light.position = bardrix::quaternion::rotate_degrees(light.position, { 0, 0, 1 }, 2);
        window.redraw();
        std::cout << (std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now() - start)).count() << "\n";
        std::this_thread::sleep_for(std::chrono::system_clock::now() - start + std::chrono::milliseconds(200));
    }
}

/*double calculate_light_intensity(const bardrix::point3& intersection_point, const bardrix::vector3& intersection_normal, const bardrix::light& light) {

    const bardrix::vector3 light_intersection_vector = intersection_point.vector_to(light.position).normalized();

    const double angle = intersection_normal.dot(light_intersection_vector);

    if (angle < 0) // This means the light is behind the intersection_point point
        return 0;

    // We use the angle and the inverse square law to calculate the intensity
    return angle * light.inverse_square_law(intersection_point);
}*/

/*
Nieuwe versie van calculate_light_intensity voor reflectie met diffuse,ambient,specular
OUDE === double projected_angle_intensity = calculate_light_intensity(intersection.value(), sphere_obj1.normal_at(intersection.value()), light);
sphere === de shape van het object die is geraakt
NIEUWE === double projected_angle_intensity = calculate_light_intensity(objects[0],light,camera,intersection.value());
*/
double calculate_light_intensity(const bardrix::shape& shape, const bardrix::light& light, const bardrix::camera& camera,
    const bardrix::point3& intersection_point) {
    const bardrix::vector3 light_intersection_vector = intersection_point.vector_to(light.position).normalized();

    // Angle between the normal and the light intersection vector
    const double angle = shape.normal_at(intersection_point).dot(light_intersection_vector);

    if (angle < 0) // This means the light is behind the intersection_point
    return 0;

    // Specular reflection
    bardrix::vector3 reflection = bardrix::quaternion::mirror(light_intersection_vector,
    shape.normal_at(intersection_point));
    double specular_angle = reflection.dot(camera.position.vector_to(intersection_point).normalized());
    double specular = std::pow(specular_angle, shape.get_material().get_shininess());

    // We're calculating phong shading (ambient + diffuse + specular)
    double intensity = shape.get_material().get_ambient();
    intensity += shape.get_material().get_diffuse() * angle;
    intensity += shape.get_material().get_specular() * specular;

    // Max intensity is 1
    return min(1.0, intensity * light.inverse_square_law(intersection_point));
}

double calc_distance(const bardrix::point3& p1, const bardrix::point3& p2) {
    return std::sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2) + pow(p2.z - p1.z, 2));
}


int main() {
    std::vector<sphere> objects;
    int width = 600;
    int height = 600;
    // Create a window
    bardrix::window window("Raytracing", width, height);

    // Create a camera
    bardrix::camera camera = bardrix::camera({ 0,0,0 }, { 0,0,1 }, width, height, 60);

    // Create a sphere
    sphere sphere_obj1(0.5, bardrix::point3(0.0, 0.0, 3.0)); // Middle ball
    sphere sphere_obj2(0.3, bardrix::point3(-0.5, 0.5, 3.0)); // Left ball
    sphere sphere_obj3(0.3, bardrix::point3(0.5, 0.5, 3.0)); // Right ball
    //Set sphere material Material = Ambient,diffuse,specular,shininess
    
    
    bardrix::material MaterialSphere1(0, 0.5, 0.7, 20);
    bardrix::material MaterialSphere2(0, 0.5, 0.7, 20);
    bardrix::material MaterialSphere3(0, 0.5, 0.7, 20);
    MaterialSphere1.color = bardrix::color::red();
    MaterialSphere2.color = bardrix::color::magenta();
    MaterialSphere3.color = bardrix::color::white();
    sphere_obj1.set_material(MaterialSphere1);
    sphere_obj2.set_material(MaterialSphere2);
    sphere_obj3.set_material(MaterialSphere3);
    // Light
    bardrix::point3 position(2, 5, 2);
    bardrix::color color = bardrix::color::red();
    double intensity = 10.0;
    bardrix::light light(position, intensity, color);
    bardrix::point3 position2(1, 1, 5);
    bardrix::color color2 = bardrix::color::magenta();
    double intensity2 = 4.0;
    //bardrix::light light2(position2, intensity2, color2);
    //threadVec.emplace_back([&light, &window] { tickFunc(light, window); });//Set light obj in tick func
    objects.emplace_back(sphere_obj1);
    objects.emplace_back(sphere_obj2);
    objects.emplace_back(sphere_obj3);

    
    window.on_paint = [&camera, &objects, &light](bardrix::window* window, std::vector<uint32_t>& buffer) {
        // Draw the scene
        for (int y = 0; y < window->get_height(); y++) {
            for (int x = 0; x < window->get_width(); x++) {
                bardrix::ray ray = *camera.shoot_ray(x, y, 10);
                bardrix::color color = bardrix::color::black();

                // Variables to keep track of the closest intersection point and object
                std::optional<bardrix::point3> closest_intersection;
                const sphere* closest_object = nullptr;
                double closest_distance = std::numeric_limits<double>::infinity();

                // Iterate over all objects to find the closest intersection
                for (const auto& obj : objects) {
                    std::optional<bardrix::point3> intersection = obj.intersection(ray);
                    if (intersection.has_value()) {
                        double distance = calc_distance(camera.position, intersection.value());
                        if (distance < closest_distance) {
                            closest_distance = distance;
                            closest_intersection = intersection;
                            closest_object = &obj;
                        }
                    }
                }

                // If there is an intersection, calculate intensity based on the closest object
                if (closest_intersection.has_value() && closest_object != nullptr) {
                    bardrix::point3 intersection_point = closest_intersection.value();
                    bardrix::vector3 normal = closest_object->normal_at(intersection_point);
                    
                    double projected_angle_intensity = 0.0;
                    for (const auto& obj : objects) {
                        projected_angle_intensity += calculate_light_intensity(obj, light, camera, intersection_point);
                    }
                    
                    // Ensure intensity is capped at 1.0
                    projected_angle_intensity = min(projected_angle_intensity, 1.0);

                    color = closest_object->get_material().color * projected_angle_intensity;
                }

                buffer[y * window->get_width() + x] = color.argb(); // ARGB is the format used by Windows API
            }
        }};

    window.on_resize = [&camera](bardrix::window* window, int width, int height) {
        // Resize the camera
        camera.set_width(width);
        camera.set_height(height);

        window->redraw(); // Redraw the window (calls on_paint)
        };

    // Get width and height of the screen
    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);

    // Show the window in the center of the screen
    if (!window.show(screen_width / 2 - width / 2, screen_height / 2 - height / 2)) {
        std::cout << GetLastError() << std::endl;
        return -1;
    }

    /*
    for (auto& t : threadVec) {
        t.detach();
    }
    */

    bardrix::window::run();

    running = false;

    for (auto& t : threadVec) {
        if (t.joinable()) {
            t.join();
        }
    }
}

#else // _WIN32

int main() {
    std::cout << "This example is only available on Windows." << std::endl;
    return 0;
}

#endif // _WIN32