#include "shader.hpp"

#include "lib/gl.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

static GLenum translate_shader_type(ShaderType type)
{
    switch (type) {
    case ShaderType::VERTEX:
        return GL_VERTEX_SHADER;
    case ShaderType::GEOMETRY:
        return GL_GEOMETRY_SHADER;
    case ShaderType::FRAGMENT:
        return GL_FRAGMENT_SHADER;
    default:
        return 0;
    }
}

Shader::Shader(ShaderType type, std::string filename)
    : gl(new GLShader()),
      type(type),
      filename(filename),
      last_load_modified(0)
{
    load();
}

Shader::~Shader()
{
    destroy();
}

void Shader::reload()
{
    destroy();
    load();
}

void Shader::attach_to(GLProgram & p)
{
    glAttachShader(p.program, gl->shader);
}

void Shader::detach_from(GLProgram & p)
{
    glDetachShader(p.program, gl->shader);
}

bool Shader::is_modified() const
{
    return (last_modified() > last_load_modified);
}

bool Shader::is_valid() const
{
    return gl->shader != 0;
}

ShaderType Shader::get_type() const
{
    return type;
}

void Shader::load()
{
    gl->type = translate_shader_type(type);

    std::string source;
    if (!read_to(source)) {
        return;
    }

    if (source.empty()) {
        std::cerr << "Shader::load: source is empty for \"" << filename << "\"." << std::endl;
    }

    make(source);

    if (is_valid()) {
        last_load_modified = last_modified();
    } else {
        last_load_modified = 0;
    }
}

bool Shader::read_to(std::string & source)
{
    std::ifstream f(filename.c_str(), std::ifstream::in | std::ifstream::binary);

    if (f.is_open()) {
        std::stringstream buffer;
        buffer << f.rdbuf();
        source = buffer.str();
        return true;
    } else {
        std::cerr << "Shader::read_to: could not read source \"" << filename << "\"." << std::endl;
        return false;
    }
}

static void print_shader_error(GLuint shader, std::string filename)
{
    GLint log_len;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);

    GLchar * log_info = new GLchar[log_len];
    glGetShaderInfoLog(shader, log_len, NULL, log_info);

    std::cerr << "Shader::make: could not compile \"" << filename << "\": " << log_info << std::endl;

    delete[] log_info;
}

void Shader::make(std::string & source)
{
    const char * shader_source = source.c_str();

    gl->shader = glCreateShader(gl->type);
    glShaderSource(gl->shader, 1, &shader_source, NULL);
    glCompileShader(gl->shader);

    GLint status;
    glGetShaderiv(gl->shader, GL_COMPILE_STATUS, &status);
    if (status ==  GL_FALSE) {
        print_shader_error(gl->shader, filename);
        gl->shader = 0;
    }
}

void Shader::destroy()
{
    if (gl->shader != 0) {
        glDeleteShader(gl->shader);
        gl->shader = 0;
    }

    last_load_modified = 0;
}

time_t Shader::last_modified() const
{
   struct stat buf;

   if (stat(filename.c_str(), &buf) == 0) {
       return buf.st_mtime;
   } else {
       std::cerr << "Shader:last_modified: could not get file status for \"" << filename << "\"." << std::endl;
       return 0;
   }
}
