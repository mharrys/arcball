#include "objloader.hpp"

#include "mesh.hpp"

#include "lib/tinyobjloader.hpp"

#include <iostream>
#include <vector>

std::unique_ptr<WorldObject> OBJLoader::load(std::string path)
{
    std::vector<tinyobj::shape_t> shapes;
    std::string err = tinyobj::LoadObj(shapes, path.c_str());
    if (!err.empty()) {
        std::cerr << "OBJLoader::load: error loading \"" << path << "\": " << err << std::endl;
        return nullptr;
    }

    std::unique_ptr<WorldObject> root(new WorldObject());

    for (auto & shape : shapes) {
        std::vector<float> & positions = shape.mesh.positions;
        std::vector<float> & normals = shape.mesh.normals;
        std::vector<unsigned int> & indices = shape.mesh.indices;

        auto mesh = std::make_shared<Mesh>();

        for (unsigned int i = 0; i < positions.size(); i += 3) {
            glm::vec3 position(positions[i], positions[i + 1], positions[i + 2]);
            mesh->positions.push_back(position);
        }

        for (unsigned int i = 0; i < normals.size(); i += 3) {
            glm::vec3 normal(normals[i], normals[i + 1], normals[i + 2]);
            mesh->normals.push_back(normal);
        }

        for (auto index : indices) {
            mesh->indices.push_back(index);
        }

        mesh->update_positions = !positions.empty();
        mesh->update_normals = !normals.empty();
        mesh->update_indices = !indices.empty();

        root->add(mesh);
    }

    return root;
}
