//Includes
#ifdef _WIN32

#include "sphere.h"
#include "window.h"
#include "cube.h"
#include "world.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <future>
#include <algorithm>
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

bool running = true;//Used to determen the status of the tick thread

//Vector for threads
std::vector<std::thread> threadVec;


//Tick function to rotate a light object (Currently not used)
void tickFunc(bardrix::light &light, bardrix::window &window) {
    while (running) {
        auto start = std::chrono::system_clock::now();
        light.position = bardrix::quaternion::rotate_degrees(light.position, { 0, 0, 1 }, 2);
        window.redraw();
        std::cout << (std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now() - start)).count() << "\n";
        std::this_thread::sleep_for(std::chrono::system_clock::now() - start + std::chrono::milliseconds(200));
    }
}

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

//Materials we use in our world
bardrix::material materials(std::string name) {
    if (name == "iron") {
        bardrix::material iron(0.0, 0.2, 0.9, 20.0);
        iron.color = bardrix::color(233, 233, 233, 255);
        return iron;
    }
    else if (name == "water") {
        bardrix::material water(0.0, 1.0, 1.0, 10.0);
        water.color = bardrix::color(30, 0, 255, 255);
        return water;
    }
    else if (name == "lava") {
        bardrix::material lava(0.0, 1.0, 1.0, 10.0);
        lava.color = bardrix::color(255, 94, 0, 255);
        return lava;
    }
    else if (name == "brick") {
        bardrix::material brick(0.0, 0.8, 0.3, 1.0);
        brick.color = bardrix::color(194, 71, 43, 255);
        return brick;
    }
    else if (name == "stone") {
        bardrix::material stone(0.0, 1.0, 0.1, 0.05);
        stone.color = bardrix::color(109, 109, 109, 255);
        return stone;
    }
    else if (name == "foliage") {
        bardrix::material foliage(0.0, 1.0, 0.0, 0.0);
        foliage.color = bardrix::color(0, 175, 23, 150);
        return foliage;
    }
    else if (name == "test") {
        bardrix::material test(0, 0.5, 0.7, 20);
        test.color = bardrix::color(255, 0, 0, 255);
        return test;
    }

    bardrix::material not_found(1.0, 1.0, 0.0, 1.0);
    not_found.color = bardrix::color(255, 0, 200, 255);
    return not_found;
}

world createWorld(bardrix::camera &camera) {
    world w("Fancy Water", bardrix::point3(10.0, 10.0, 10.0), false, false);//Open-world with volume 10x10x10 without a sun.

    bardrix::light globalLight(bardrix::point3(-1, 0, 0), 4, bardrix::color::white());
    sphere s1(0.5, bardrix::point3(0.0, 0.0, 3.0));
    cube c1(0.5, bardrix::point3(0.0, 1.0, 3.0));

    s1.set_material(materials("brick"));
    c1.set_material(materials("iron"));

    w.setCamera(camera);
    w.addLight(globalLight);
    w.addObject(std::make_unique<sphere>(s1));
    w.addObject(std::make_unique<cube>(c1));

    return w;
}

int main() {
    int width = 600;
    int height = 600;

    //Create window
    bardrix::window window("fancy-water", width, height);

    // Create a camera
    bardrix::camera camera = bardrix::camera({ -1,0,0 }, { 0,0,1 }, width, height, 60);

    //Create a world object
    world fancy_world = createWorld(camera);
    
    //Paint eventlistener
    window.on_paint = [&fancy_world](bardrix::window* window, std::vector<uint32_t>& buffer) {
        
        // Draw the scene
        for (int y = 0; y < window->get_height(); y++) {
            for (int x = 0; x < window->get_width(); x++) {
                bardrix::ray ray = *fancy_world.getCamera().shoot_ray(x, y, 10);
                bardrix::color color = bardrix::color::black();

                // Variables to keep track of the closest intersection point and object
                std::optional<bardrix::point3> closest_intersection;
                const bardrix::shape* closest_object = nullptr;//Sphere obj
            
                double closest_distance = std::numeric_limits<double>::infinity();
                //const cube* closest_cube = nullptr;

                // Iterate over all objects to find the closest intersection
                for (const auto& obj : fancy_world.getObjects()) {
                    std::optional<bardrix::point3> intersection = obj->intersection(ray);
                    if (intersection.has_value()) {
                        double distance = calc_distance(fancy_world.getCamera().position, intersection.value());
                        if (distance < closest_distance) {
                            closest_distance = distance;
                            closest_intersection = intersection;
                            closest_object = obj.get();
                        }
                    }
                }

                // If there is an intersection, calculate intensity based on the closest object
                if (closest_intersection.has_value() && closest_object != nullptr) {
                    bardrix::point3 intersection_point = closest_intersection.value();
                    bardrix::vector3 normal = closest_object->normal_at(intersection_point);
                    
                    for (bardrix::light& l : fancy_world.getLights()) {
                        double projected_angle_intensity = calculate_light_intensity(*closest_object, l, fancy_world.getCamera(), intersection_point);
                        color += l.color.blended(closest_object->get_material().color) * projected_angle_intensity;
                    }
                }

                buffer[y * window->get_width() + x] = color.argb(); // ARGB is the format used by Windows API
            }
        }};
    window.on_keydown = [&fancy_world](bardrix::window* window, WPARAM key) {
        switch (key)
        {
            // Case voor input A
        case 0x41:
            fancy_world.camera.position.x += 0.1;
            break;
            // Case input D
        case 0x44:
            fancy_world.camera.position.x += 0.1;
            break;
            // Case input S
        case 0x53:
            fancy_world.camera.position.z -= 0.1;
            break;
            // Case input W
        case 0x57:
            fancy_world.camera.position.z -= 0.1;
            break;
        default:
            break;
        }
        window->redraw();
        };

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

    


    bardrix::window::run();

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
