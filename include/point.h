#ifndef POINT_H
#define POINT_H

#include <vector>
#include <glm/glm.hpp>

class Point{
    public:
        Point();
        Point(glm::vec3 coord, glm::vec3 norm, float u, float v);

        glm::vec3 coord;
        glm::vec3 norm;
        float u, v;
};

Point::Point(){

}


Point::Point(glm::vec3 coord, glm::vec3 norm, float u, float v){
    this->coord = coord;
    this->norm = norm;
    this->u = u;
    this->v = v;
}














#endif
