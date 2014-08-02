#ifndef CAMERA_HPP_INCLUDED
#define CAMERA_HPP_INCLUDED

#include "worldobject.hpp"

class Camera : public WorldObject {
public:
    glm::mat4 view() const;
    virtual glm::mat4 projection() const = 0;
};

#endif
