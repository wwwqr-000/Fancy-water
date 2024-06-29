#pragma once
#include "point2.hpp"
#include <bardrix/point3.h>
#include "cube.h"
#include "sphere.h"

class betterTexture {
    private:
        bardrix::point3 dimention;
        bardrix::material material_;
        bardrix::point3 position_;
        std::string imgPath;

    public:
        //Minimal init
        betterTexture(bardrix::point3& dimention, bardrix::material& material_, bardrix::point3& position_) : dimention(dimention), material_(material_), position_(position_) {}
        //Fully init
     

        point2 getCubeIntersectionCoords(const bardrix::point3& intersection, cube &c) {
            bardrix::vector3 normal = cube::normal_at(intersection);
            point2 uv;

            if (normal.x == 1 || normal.x == -1) {
                uv.x = (intersection.z - (position_.z - dimention.z * 0.5)) / dimention.z;
                uv.y = (intersection.y - (position_.y - dimention.y * 0.5)) / dimention.y;
            }
            else if (normal.y == 1 || normal.y == -1) {
                uv.x = (intersection.x - (position_.x - dimention.x * 0.5)) / dimention.x;
                uv.y = (intersection.z - (position_.z - dimention.z * 0.5)) / dimention.z;
            }
            else if (normal.z == 1 || normal.z == -1) {
                uv.x = (intersection.x - (position_.x - dimention.x * 0.5)) / dimention.x;
                uv.y = (intersection.y - (position_.y - dimention.y * 0.5)) / dimention.y;
            }

            return uv;
        }
};