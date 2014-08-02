#include "program.hpp"

#include "lib/gl.hpp"

#include <iostream>

Program::Program(std::string vert_filename, std::string frag_filename)
    : gl(new GLProgram()),
      vert(ShaderType::VERTEX, vert_filename),
      frag(ShaderType::FRAGMENT, frag_filename)
{
    load();
}

Program::~Program()
{
    destroy();
}

void Program::use()
{
    glUseProgram(gl->program);
}

void Program::reload()
{
    if (vert.is_modified()) {
        vert.reload();
    }

    if (frag.is_modified()) {
        frag.reload();
    }

    make();
}

bool Program::is_modified() const
{
    return vert.is_modified() || frag.is_modified();
}

bool Program::is_valid() const
{
    return gl->program > 0 && vert.is_valid() && frag.is_valid();
}

static GLint get_uniform_location(GLuint program, const GLchar * name)
{
    GLint location = glGetUniformLocation(program, name);

    if (location == -1) {
        std::cerr << "Program::get_uniform_location: could not get uniform location for \"" << name << "\"." << std::endl;
    }

    return location;
}

void Program::set_uniform(std::string name, int value)
{
    GLint location = get_uniform_location(gl->program, name.c_str());
    glUniform1i(location, value);
}

void Program::set_uniform(std::string name, bool value)
{
    set_uniform(name, value ? 1 : 0);
}

void Program::set_uniform(std::string name, float value)
{
    GLint location = get_uniform_location(gl->program, name.c_str());
    glUniform1f(location, value);
}

void Program::set_uniform(std::string name, glm::mat3 const & value)
{
    GLint location = get_uniform_location(gl->program, name.c_str());
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Program::set_uniform(std::string name, glm::mat4 const & value)
{
    GLint location = get_uniform_location(gl->program, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Program::set_uniform(std::string name, glm::vec2 const & value)
{
    GLint location = get_uniform_location(gl->program, name.c_str());
    glUniform2f(location, value[0], value[1]);
}

void Program::set_uniform(std::string name, glm::vec3 const & value)
{
    GLint location = get_uniform_location(gl->program, name.c_str());
    glUniform3f(location, value[0], value[1], value[2]);
}

void Program::set_uniform(std::string name, glm::vec4 const & value)
{
    GLint location = get_uniform_location(gl->program, name.c_str());
    glUniform4f(location, value[0], value[1], value[2], value[3]);
}

void Program::load()
{
    gl->program = glCreateProgram();
    if (gl->program == 0) {
        std::cerr << "Program::load: could not create program handle." << std::endl;
        return;
    }

    make();
}

static void print_program_error(GLuint program)
{
    GLint log_len;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);

    GLchar * log_info = new GLchar[log_len];
    glGetProgramInfoLog(program, log_len, NULL, log_info);

    std::string error_msg = "Program::make: could not load program: ";
    std::cerr << error_msg << log_info << std::endl;

    delete[] log_info;
}

void Program::make()
{
    if (!vert.is_valid()) {
        std::cerr << "Program::make: vertex shader is not valid." << std::endl;
        return;
    }

    if (!frag.is_valid()) {
        std::cerr << "Program::make: fragment shader is not valid." << std::endl;
        return;
    }

    vert.attach_to(*gl.get());
    frag.attach_to(*gl.get());

    // associate vertex attribute index with a shder input variable
    glBindAttribLocation(gl->program, VERTEX_POSITION_INDEX, "vertex_position");
    glBindAttribLocation(gl->program, VERTEX_COLOR_INDEX, "vertex_color");
    glBindAttribLocation(gl->program, VERTEX_NORMAL_INDEX, "vertex_normal");
    glBindAttribLocation(gl->program, VERTEX_TEX_COORD_INDEX, "vertex_tex_coord");

    glLinkProgram(gl->program);

    GLint status;
    glGetProgramiv(gl->program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        print_program_error(gl->program);
    }

    // detach shaders now before their raw handle becomes obsolete from
    // reloading modified shaders, this is safe to do now since the shaders
    // are already linked to the program
    vert.detach_from(*gl.get());
    frag.detach_from(*gl.get());
}

void Program::destroy()
{
    if (is_valid()) {
        glDeleteProgram(gl->program);
        gl->program = 0;
    }
}
