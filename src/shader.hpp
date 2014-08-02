#ifndef SHADER_HPP_INCLUDED
#define SHADER_HPP_INCLUDED

#include <memory>
#include <string>
#include <sys/stat.h>

struct GLProgram;
struct GLShader;

enum class ShaderType {
    VERTEX,
    GEOMETRY,
    FRAGMENT
};

class Shader {
public:
    Shader(ShaderType type, std::string filename);
    ~Shader();

    void reload();

    void attach_to(GLProgram & p);
    void detach_from(GLProgram & p);

    bool is_modified() const;
    bool is_valid() const;

    ShaderType get_type() const;
private:
    void load();
    bool read_to(std::string & source);
    void make(std::string & source);
    void destroy();

    time_t last_modified() const;

    std::unique_ptr<GLShader> gl;

    ShaderType type;
    std::string filename;

    time_t last_load_modified;
};

#endif
