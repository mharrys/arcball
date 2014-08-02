#include "camera.hpp"

glm::mat4 Camera::view() const
{
    return glm::inverse(world_transform());
}
