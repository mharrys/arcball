#include "worldobject.hpp"

#include <iostream>

WorldObject::WorldObject()
    : parent(nullptr)
{
}

void WorldObject::draw()
{
}

void WorldObject::add(std::shared_ptr<WorldObject> object)
{
    children.push_back(object);
    object->parent = this;
}

void WorldObject::traverse(std::function<void(WorldObject &)> callback)
{
    callback(*this);

    for (auto child : children) {
        child->traverse(callback);
    }
}

void WorldObject::translate_x(float distance)
{
    translate(distance, X_UNIT);
}

void WorldObject::translate_y(float distance)
{
    translate(distance, Y_UNIT);
}

void WorldObject::translate_z(float distance)
{
    translate(distance, Z_UNIT);
}

void WorldObject::rotate_x(float angle_deg)
{
    rotate(angle_deg, X_UNIT);
}

void WorldObject::rotate_y(float angle_deg)
{
    rotate(angle_deg, Y_UNIT);
}

void WorldObject::rotate_z(float angle_deg)
{
    rotate(angle_deg, Z_UNIT);
}

void WorldObject::update_local_transform()
{
    glm::mat4 T = glm::translate(glm::mat4(1.0f), position);
    glm::mat4 R = glm::toMat4(orientation);

    local = T * R;
}

void WorldObject::update_world_transform()
{
    update_local_transform();

    if (parent == nullptr) {
        world = local;
    } else {
        world = parent->world * local;
    }

    for (auto child : children) {
        child->update_world_transform();
    }
}

glm::mat4 WorldObject::local_transform() const
{
    return local;
}

glm::mat4 WorldObject::world_transform() const
{
    return world;
}

void WorldObject::translate(float distance, glm::vec3 axis)
{
    position += orientation * (axis * distance);
}

void WorldObject::rotate(float angle_deg, glm::vec3 axis)
{
    orientation = glm::rotate(orientation, angle_deg, axis);
}
