#include "orthocamera.hpp"

OrthoCamera::OrthoCamera(float left, float right, float bottom, float top)
    : OrthoCamera(left, right, bottom, top, -1.0f, 1.0f)
{
}

OrthoCamera::OrthoCamera(float left, float right, float bottom, float top, float near, float far)
    : left(left),
      right(right),
      bottom(bottom),
      top(top),
      near(near),
      far(far)
{
}

glm::mat4 OrthoCamera::projection() const
{
    return glm::ortho(left, right, bottom, top, near, far);
}
