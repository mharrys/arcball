#ifndef PERSPECTIVECAMERA_HPP_INCLUDED
#define PERSPECTIVECAMERA_HPP_INCLUDED

#include "camera.hpp"

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera();
    PerspectiveCamera(float fov, float aspect_ratio, float near, float far);

    glm::mat4 projection() const override;

    float fov;
    float aspect_ratio;
    float near;
    float far;
};

#endif
