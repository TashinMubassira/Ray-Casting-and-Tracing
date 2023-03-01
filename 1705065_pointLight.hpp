#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "1705065_Point.hpp"

class pointLight{

    public:
        point light_pos;
        double color[3];

        pointLight(){}

        pointLight(point pos,double r, double g, double b){
            this->light_pos.x = pos.x;
            this->light_pos.y = pos.y;
            this->light_pos.z = pos.z;
            this->color[0] = r;
            this->color[1] = g;
            this->color[2] = b;
        }
};

#endif