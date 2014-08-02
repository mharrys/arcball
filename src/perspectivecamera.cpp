#include "perspectivecamera.hpp"

PerspectiveCamera::PerspectiveCamera()
    : PerspectiveCamera(45.0f, 1.0, 0.1f, 1000.0f)
{
}

PerspectiveCamera::PerspectiveCamera(float fov, float aspect_ratio, float near, float far)
    : fov(fov),
      aspect_ratio(aspect_ratio),
      near(near),
      far(far)
{
}

glm::mat4 PerspectiveCamera::projection() const
{
    return glm::perspective(fov, aspect_ratio, near, far);
}

