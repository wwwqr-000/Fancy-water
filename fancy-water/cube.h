#include <bardrix/objects.h>

#pragma once

class cube : public bardrix::shape {//Creating a test cube based on the sphere header
    protected:

        bardrix::point3 dimention;
        bardrix::material material_;
        bardrix::point3 position_;

    public:
        cube();
        explicit cube(const bardrix::point3 &dimention);
        cube(const bardrix::point3& dimention, const bardrix::point3& position);

        cube(const bardrix::point3& dimention, const bardrix::point3& position, const bardrix::material& material);

        NODISCARD const bardrix::material& get_material() const override;
        NODISCARD const bardrix::point3& get_position() const override;
        void set_material(const bardrix::material& material) override;
        void set_position(const bardrix::point3& position) override;

        NODISCARD bardrix::vector3 normal_at(const bardrix::point3& intersection) const override;

        NODISCARD std::optional<bardrix::point3> intersection(const bardrix::ray& ray) const override;
};
