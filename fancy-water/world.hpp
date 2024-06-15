#pragma once

#include <bardrix/light.h>
#include <bardrix/camera.h>
#include <bardrix/objects.h>

class world {
    private:
        std::string name;
        std::vector<std::unique_ptr<bardrix::shape>> objects;
        std::vector<bardrix::light> lights;
        bool sun, box;
        bardrix::point3 dimBorder;

    public:
        bardrix::camera camera;

        //Bare minimum.
        world(std::string name, bardrix::point3 dimBorder, bool sun, bool box) : name(name), dimBorder(dimBorder), sun(sun), box(box) {}

        //Fully init.
        world(std::string name, std::vector<std::unique_ptr<bardrix::shape>> objects, std::vector<bardrix::light> lights, bardrix::point3 dimBorder, bool sun, bool box, bardrix::camera) : name(name), objects(std::move(objects)), lights(lights), dimBorder(dimBorder), sun(sun), box(box), camera(camera) {}

        //Returns all the objects (shapes) from the object vector.
        std::vector<std::unique_ptr<bardrix::shape>> &getObjects() { return this->objects; }

        //Add a object (shape) to the object vector.
        void addObject(std::unique_ptr<bardrix::shape> obj) { this->objects.push_back(std::move(obj)); }

        //Remove a object from the object vector, based on the object itself.
        /*
        bool removeObject(bardrix::shape &object) {
            int c = -1;
            for (auto& obj : this->objects) {
                ++c;
                if (obj == object) {
                    this->objects.erase(this->objects.begin() + c);
                    return true;
                }
            }
            return false;
        }*///The shape class needs a auto-increment member to make this unique and work.

        //Get the name of the world.
        std::string getName() { return this->name; }

        //Check if the world has a sun.
        bool hasSun() { return this->sun; }

        //Check if the world is box-based.
        bool isBox() { return this->box; }

        //Add a light to the light vector.
        void addLight(bardrix::light light) { this->lights.emplace_back(light); }
        std::vector<bardrix::light> getLights() { return this->lights; }

        //Remove a light from the light vector, based on itself.
        /*
        bool removeLight(bardrix::light light) {
            int c = -1;
            for (auto& s : this->lights) {
                ++c;
                if (s == light) {
                    this->lights.erase(this->lights.begin() + c);
                    return true;
                }
            }
            return false;
        }*///The shape class needs a auto-increment member to make this unique and work.

        //Camera getter and setter
        void setCamera(bardrix::camera& camera) { this->camera = camera; }
        bardrix::camera getCamera() { return this->camera; }
        //
};
