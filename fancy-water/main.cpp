//
// Created by Bardio on 22/05/2024.
//

#include <iostream>
#include <thread>
#include <vector>
#include <future>

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

double calculate_light_intensity(const bardrix::point3& intersection_point, const bardrix::vector3& intersection_normal, const bardrix::light& light) {

    const bardrix::vector3 light_intersection_vector = intersection_point.vector_to(light.position).normalized();

    const double angle = intersection_normal.dot(light_intersection_vector);

    if (angle < 0) // This means the light is behind the intersection_point point
        return 0;

    // We use the angle and the inverse square law to calculate the intensity
    return angle * light.inverse_square_law(intersection_point);
}

int main() {
    int width = 600;
    int height = 600;
    // Create a window
    bardrix::window window("Raytracing", width, height);

    // Create a camera
    bardrix::camera camera = bardrix::camera({ 0,0,0 }, { 0,0,1 }, width, height, 60);

    // Create a sphere
    sphere sphere(1.0, bardrix::point3(0.0, 0.0, 3.0));

    //here light plz
    bardrix::point3 position(0, 4, 2);
    bardrix::color color = bardrix::color::red();
    double intensity = 4.0;
    bardrix::light light(position, intensity, color);
    threadVec.emplace_back([&light, &window] { tickFunc(light, window); });//Set light obj in tick func

    
    window.on_paint = [&camera, &sphere, &light](bardrix::window* window, std::vector<uint32_t>& buffer) {
        // Draw the sphere
        for (int y = 0; y < window->get_height(); y++) {
            for (int x = 0; x < window->get_width(); x++) {
                bardrix::ray ray = *camera.shoot_ray(x, y, 10);
                std::optional<bardrix::point3> intersection = sphere.intersection(ray);
                bardrix::color color = bardrix::color::black();

                //if intensity needs a light then how can calculating the intensity need a light 

                // If the ray intersects the sphere, paint the pixel white
                if (intersection.has_value()) {


                    double projected_angle_intensity = calculate_light_intensity(intersection.value(), sphere.normal_at(intersection.value()), light);

                    color = bardrix::color::white() * projected_angle_intensity;

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