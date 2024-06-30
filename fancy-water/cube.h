#pragma once
#include <bardrix/objects.h>
#include "point2.hpp"

class cube : public bardrix::shape {//Creating a test cube based on the sphere header
    protected:

        bardrix::point3 dimension;
        bardrix::material material_;
        bardrix::point3 position_;
        std::string type;

    public:
        cube();
        explicit cube(const bardrix::point3 &dimension);
        cube(const bardrix::point3& dimension, const bardrix::point3& position);

        cube(const bardrix::point3& dimension, const bardrix::point3& position, const bardrix::material& material);

        cube(const bardrix::point3& dimension, const bardrix::point3& position, const bardrix::material& material, const std::string type);

        NODISCARD const bardrix::material& get_material() const override;
        NODISCARD const bardrix::point3& get_position() const override;
        void set_material(const bardrix::material& material) override;
        void set_position(const bardrix::point3& position) override;

        NODISCARD bardrix::vector3 normal_at(const bardrix::point3& intersection) const override;

        NODISCARD std::optional<bardrix::point3> intersection(const bardrix::ray& ray) const override;

        //Get texture coordinates from intersection hit
        NODISCARD point2 getCubeIntersectionCoords(const bardrix::point3& intersection) const;
        //
        
        //Get texture path from this cube.
        NODISCARD std::string getType() const;
        //
};
