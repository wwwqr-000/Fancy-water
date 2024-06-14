
class world {
    private:
        std::string name;
        std::vector<std::unique_ptr<bardrix::shape>> objects;
        std::vector<bardrix::light> lights;
        bool sun, box;
        bardrix::point3 dimBorder;

    public:
        //Constructor overloading.
        world(std::string name, bardrix::point3 dimBorder, bool sun, bool box) : name(name), dimBorder(dimBorder), sun(sun), box(box) {}
        world(std::string name, std::vector<std::unique_ptr<bardrix::shape>> objects, bardrix::point3 dimBorder, bool sun, bool box) : name(name), objects(objects), dimBorder(dimBorder), sun(sun), box(box) {}

        //Returns all the objects (shapes) from the object vector.
        std::vector<std::unique_ptr<bardrix::shape>> getObjects() { return this->objects; }

        //Add a object (shape) to the object vector.
        void addObject(bardrix::shape &obj) { this->objects.emplace_back(std::make_unique<shape>(obj)); }
        void importObjects(std::vector<std::unique_ptr<bardrix::shape>> objectVec) {

        }

        bool removeObject(bardrix::shape &object) {
            for (int i = 0; i < this->objects.size(); i++) {
                if (objects[i] == object) {
                    this->objects.erase(this->objects.begin() + i);
                    return true;
                }
            }
            return false;
        }

        std::string getName() { return this->name; }
        bool hasSun() { return this->sun; }
        bool isBox() { return this->box; }

};
