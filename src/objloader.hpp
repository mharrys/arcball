#ifndef OBJLOADER_HPP_INCLUDED
#define OBJLOADER_HPP_INCLUDED

#include <memory>
#include <string>

class WorldObject;

class OBJLoader {
public:
    static std::unique_ptr<WorldObject> load(std::string path);
};

#endif
