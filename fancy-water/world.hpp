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
        size_t renderDistance;
        bardrix::point3 dimBorder;

    public:
        bardrix::camera camera;

        //Bare minimum.
        world(std::string name, bardrix::point3 dimBorder, bool sun, bool box, size_t renderDistance) : name(name), dimBorder(dimBorder), sun(sun), box(box), renderDistance(renderDistance) {}

        //Fully init.
        world(std::string name, std::vector<std::unique_ptr<bardrix::shape>> objects, std::vector<bardrix::light> lights, bardrix::point3 dimBorder, bool sun, bool box, bardrix::camera, size_t renderDistance) : name(name), objects(std::move(objects)), lights(lights), dimBorder(dimBorder), sun(sun), box(box), camera(camera), renderDistance(renderDistance) {}

        //Returns all the objects (shapes) from the object vector.
        std::vector<std::unique_ptr<bardrix::shape>> &getObjects() { return this->objects; }

        //Add a object (shape) to the object vector.
        void addObject(std::unique_ptr<bardrix::shape> obj) { this->objects.push_back(std::move(obj)); }

        //Get the name of the world.
        std::string getName() { return this->name; }

        //Check if the world has a sun.
        bool hasSun() { return this->sun; }

        //Check if the world is box-based.
        bool isBox() { return this->box; }

        //Add a light to the light vector.
        void addLight(bardrix::light light) { this->lights.emplace_back(light); }
        std::vector<bardrix::light> getLights() { return this->lights; }

        //Camera getter and setter
        void setCamera(bardrix::camera& camera) { this->camera = camera; }
        bardrix::camera getCamera() { return this->camera; }
        //

        //Getter and setter renderDistance
        size_t getRenderDistance() { return this->renderDistance; }
        void setRenderDistance(size_t rDist) { this->renderDistance = rDist; }
        //
};
