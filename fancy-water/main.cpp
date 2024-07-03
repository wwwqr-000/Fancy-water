//Includes
#ifdef _WIN32

#include "point2.hpp"
#include "point3_int.hpp"
#include "sphere.h"
#include "window.h"
#include "cube.h"
#include "world.hpp"
#include "betterTexture.hpp"

#include <cmath>
#include <iostream>
#include <thread>
#include <vector>
#include <future>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <utility>
#include <bardrix/ray.h>
#include <bardrix/light.h>
#include <bardrix/camera.h>
#include <bardrix/color.h>
#include <filesystem>

// Undefine any macros that might conflict
#undef min
#undef max
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
std::vector<std::vector<bardrix::point3>> water_surface; // 2D-array om punten op het wateroppervlak te definiëren



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
double calc_distance(const bardrix::point3& p1, const bardrix::point3& p2) {
    return std::sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2) + pow(p2.z - p1.z, 2));
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

    if (angle < 0) // This means the light is behind the intersection_point or not visible
        return 0; // Return 0 (black) if the light is not visible

    // Specular reflection
    const bardrix::vector3 normal = shape.normal_at(intersection_point);
    bardrix::vector3 reflection = light_intersection_vector - 2 * (light_intersection_vector.dot(normal)) * normal;
    double specular_angle = reflection.dot(camera.position.vector_to(intersection_point).normalized());
    double specular = std::pow(specular_angle, shape.get_material().get_shininess());

    // We're calculating phong shading (ambient + diffuse + specular)
    double intensity = shape.get_material().get_ambient();
    intensity += shape.get_material().get_diffuse() * angle;
    intensity += shape.get_material().get_specular() * specular;

    // Max intensity is 1
    //std::cout << intensity << "\n";
    return std::min(1.0, intensity * light.inverse_square_law(intersection_point));
}



//Materials we use in our world
bardrix::material materials(std::string name) {
    if (name == "iron") {
        bardrix::material iron(0.0, 0.5, 0.7, 20);
        iron.color = bardrix::color(233, 233, 233, 255);
        return iron;
    }
    else if (name == "water") {
        bardrix::material water(0.0, 1.0, 1.0, 10.0);
        water.color = bardrix::color(0, 16, 255, 255);
        return water;
    }
    else if (name == "dirt") {
        bardrix::material dirt(1.0, 1.0, 1.0, 1.0);
        dirt.color = bardrix::color(0, 0, 0, 255);
        return dirt;
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

std::vector<std::string> explode(std::string const& s, char delim) {//Explode a string
    std::vector<std::string> result;
    std::istringstream iss(s);

    for (std::string token; std::getline(iss, token, delim); ) {
        result.push_back(std::move(token));
    }

    return result;
}

cube blocks(std::string& name, bardrix::point3& pos) {
    bardrix::point3 cs(1.0, 1.0, 1.0);
    if (name == "iron_block") {
        return cube{ cs, pos, materials("iron"), "iron_block"};
    }
    else if (name == "dirt") {
        return cube{ cs, pos, materials("dirt"), "dirt" };
    }
}

std::vector<cube> importStructure(std::string path) {//Create a cube vector based on raw text inside a .structure file
    std::vector<cube> cubes;
    std::ifstream structure(path);
    std::string line;
    while (std::getline(structure, line)) {
        try {
            std::vector<std::string> arr = explode(line, ',');
            cube c = blocks(arr[3], bardrix::point3(std::stod(arr[0]), std::stod(arr[1]), std::stod(arr[2])));
            cubes.emplace_back(c);
        }
        catch (std::exception) {
            std::cout << "Error reading structure at '" << path << "'\n";
        }
    }

    structure.close();
    return cubes;
}

world createWorld(bardrix::camera &camera) {
    world w("Fancy Water", bardrix::point3(10.0, 10.0, 10.0), false, false, 100);//Open-world with volume 10x10x10 without a sun. (renderDistance=10)

    bardrix::light globalLight(bardrix::point3(-0.1, 10.0, -4.3), 200, bardrix::color::white());
    cube floor(bardrix::point3(10.0, 0.1, 10.0), bardrix::point3(0.0, -5.0, 0.0));

    //Create structures based on structure files from structures folder
    for (auto& entr : std::filesystem::directory_iterator("../../../project_assets/structures")) {
        for (auto& obj : importStructure(entr.path().string())) {
            w.addObject(std::make_unique<cube>(obj));
        }
    }
    //

    floor.set_material(materials("iron"));

    w.setCamera(camera);
    w.addLight(globalLight);
    w.addObject(std::make_unique<cube>(floor));

    return w;
}

std::vector<std::string> loadedTextures;

int loadPixels(betterTexture& texture_mask, std::string& type) {
    int stat = -1;
    bool exists = false;
    for (auto& t : loadedTextures) {
        if (t == type) {
            exists = true;
            break;
        }
    }
    if (!exists) {
        texture_mask.createPixels(stat);
        loadedTextures.emplace_back(type);
    }

    return stat;
}

//Create log all the betterTexture objects
std::vector<betterTexture> texturesVec;

void createTextures(std::string& texture_loc) {//Load all textures
    betterTexture iron(texture_loc + "iron_block.bmp");
    betterTexture dirt(texture_loc + "dirt.bmp");
    int stat = -1;
    iron.createPixels(stat);
    dirt.createPixels(stat);

    texturesVec.emplace_back(iron);
    texturesVec.emplace_back(dirt);
}

betterTexture texture(std::string& imgPath, std::string& texture_loc) {//Get a texture obj
    for (auto& obj : texturesVec) {
        if (obj.getBMP() == imgPath) {
;            return obj;
        }
    }
    return betterTexture{ texture_loc + "missing.bmp" };
}
//

void update_texture_mask(betterTexture& texture_mask, std::string& imgPath, std::string& texture_loc) {
    if (texture_mask.getBMP() != (texture_loc + imgPath)) {
        texture_mask = texture(texture_loc + imgPath, texture_loc);
    }
}

void faceTheFaces(betterTexture& texture_mask, std::string& texture_loc, std::string& type, point2& pixelCoord) {//Used to give cube faces textures
    if (type == "iron_block") {
        update_texture_mask(texture_mask, std::string("iron_block.bmp"), texture_loc);
    }
    else if (type == "grass_block") {

        if (pixelCoord.face == "top") {
            texture_mask.setBMP(texture_loc + "grass_top.bmp");
            return;
        }
        else if (pixelCoord.face == "bottom") {
            texture_mask.setBMP(texture_loc + "dirt.bmp");
            return;
        }
        else {
            texture_mask.setBMP(texture_loc + "grass_block_side.bmp");
            return;
        }
    }
    else if (type == "dirt") {
        update_texture_mask(texture_mask, std::string("dirt.bmp"), texture_loc);
    }
}

int main() {
    int width = 600;
    int height = 600;

    //Create window
    bardrix::window window("fancy-water", width, height);

    // Create a camera
    bardrix::camera camera = bardrix::camera({-0.1, 2.2, -4.6}, {0, 0, 1}, width, height, 60);

    //Create a world object
    world fancy_world = createWorld(camera);

    //Paint eventlistener
    window.on_paint = [&fancy_world](bardrix::window* window, std::vector<uint32_t>& buffer) {
        
        // Draw the scene
        std::string texture_loc = "../../../project_assets/textures/";
        //Initialize textures and global mask
        betterTexture texture_mask(texture_loc + "missing.bmp");
        int stat = 0;
        texture_mask.createPixels(stat);
        createTextures(texture_loc);//Load all bmp files into pixel structed vectors
        //
        for (int y = 0; y < window->get_height(); y++) {
            for (int x = 0; x < window->get_width(); x++) {
                bardrix::ray ray = *fancy_world.getCamera().shoot_ray(x, y, fancy_world.getRenderDistance());
                bardrix::color color(255, 255, 255, 255);
                //water.render(buffer, 20, 20);
                // Variables to keep track of the closest intersection point and object
                std::optional<bardrix::point3> closest_intersection;
                const bardrix::shape* closest_object = nullptr;
            
                double closest_distance = std::numeric_limits<double>::infinity();
                //const cube* closest_cube = nullptr;

                //Iterate over all objects to find the closest intersection
                for (const auto& obj : fancy_world.getObjects()) {
                    std::optional<bardrix::point3> intersection = obj->intersection(ray);
                    if (intersection.has_value()) {
                        double distance = fancy_world.getCamera().position.distance_squared(intersection.value());
                        if (distance < closest_distance) {
                            closest_distance = distance;
                            closest_intersection = intersection;
                            closest_object = obj.get();
                        }
                    }
                }

                //If there is an intersection, calculate intensity based on the closest object
                if (closest_intersection.has_value() && closest_object != nullptr) {
                    //Cout the coords of intersecion
                    //std::cout << closest_object.
                    bardrix::point3 intersection_point = closest_intersection.value();
                    bardrix::vector3 normal = closest_object->normal_at(intersection_point);

                    //Check if the current obj is a cube.
                    const cube* closest_cube = dynamic_cast<const cube*>(closest_object);
                    if (closest_cube && closest_cube->getType() != "") {
                        //Get texture color based on pixel coordinate
                        point2 pixelCoord = closest_cube->getCubeIntersectionCoords(closest_intersection.value());
                        faceTheFaces(texture_mask, texture_loc, closest_cube->getType(), pixelCoord);
                        point3_int RGB = texture_mask.getPixelValue(pixelCoord.x, pixelCoord.y);
                        bardrix::color pColor(RGB.x, RGB.y, RGB.z, 255);
                        //
                        for (bardrix::light& l : fancy_world.getLights()) {
                            double projected_angle_intensity = calculate_light_intensity(*closest_object, l, fancy_world.getCamera(), intersection_point);
                            color = pColor * projected_angle_intensity;
                        }
                    }
                    //
                    else {
                        for (bardrix::light& l : fancy_world.getLights()) {
                            double projected_angle_intensity = calculate_light_intensity(*closest_object, l, fancy_world.getCamera(), intersection_point);
                            color += l.color.blended(closest_object->get_material().color) * projected_angle_intensity;
                        }
                    }
                }
                buffer[y * window->get_width() + x] = color.argb(); // ARGB is the format used by Windows API
            }
        }    
    };
    window.on_keydown = [&fancy_world](bardrix::window* window, WPARAM key) {
        //std::cout << key;//Key logger 
        switch (key) {
            case 0x41: // A-toets
                fancy_world.camera.position.x += 0.1;
                break;
            case 0x53: // S-toets
                fancy_world.camera.position.y -= 0.1;
                break;
            case 0x44: // D-toets
                fancy_world.camera.position.x -= 0.1;
                break;
            case 0x57: // W-toets
                fancy_world.camera.position.y += 0.1;
                break;
            case 0x51: // Q-toets
                fancy_world.camera.position.z -= 0.1;
                break;
            case 0x45: // E-toets
                fancy_world.camera.position.z += 0.1;
                break;
        }
        std::cout << "x:" << fancy_world.camera.position.x << "\ny:" << fancy_world.camera.position.y << "\nz:" << fancy_world.camera.position.z << "\n";
            /*
            case 13: //Enter
                std::cout << "13???";
                cube newCube(bardrix::point3(0.5, 0.5, 0.5), bardrix::point3(0.0, 1.0, 3.0));
                newCube.set_material(materials("water")); // Example material, change as needed
                fancy_world.addObject(std::make_unique<cube>(newCube));
                break;
            */
            /*//This only works if the camera.h class did not get reloaded with CMake.
            case 37: //Linker pijl
                //std::cout << "test1\n";
                fancy_world.camera.rotate_yaw(-5);
                break;
            case 38: //Boven
                fancy_world.camera.rotate_pitch(-5);
                break;
            case 39: //Rechts
                fancy_world.camera.rotate_yaw(5);
                break;    
            case 40: //Beneden toets
                fancy_world.camera.rotate_pitch(5);
                break;
            break;
            */
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