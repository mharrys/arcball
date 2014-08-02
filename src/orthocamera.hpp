#ifndef ORTHOCAMERA_HPP_INCLUDED
#define ORTHOCAMERA_HPP_INCLUDED

#include "camera.hpp"

class OrthoCamera : public Camera {
public:
    OrthoCamera(float left, float right, float bottom, float top);
    OrthoCamera(float left, float right, float bottom, float top, float near, float far);

    glm::mat4 projection() const override;

    float left;
    float right;
    float bottom;
    float top;
    float near;
    float far;
};

#endif
