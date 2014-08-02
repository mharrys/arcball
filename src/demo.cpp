#include "demo.hpp"

#include "objloader.hpp"

#include "lib/gl.hpp"

#include <iostream>

Demo::Demo()
    : light_program("assets/shaders/light.vert", "assets/shaders/light.frag"),
      basic_program("assets/shaders/basic.vert", "assets/shaders/basic.frag"),
      light_direction(0.0f, 1.0f, 1.0f, 0.0f),
      helper_camera(-1.0f, 1.0f, -1.0f, 1.0f),
      show_helpers(true)
{
}

bool Demo::create(std::shared_ptr<Window> window)
{
    this->window = window;

    if (!light_program.is_valid()) {
        std::cerr << "Demo::on_create: light program is invalid." << std::endl;
        return false;
    }

    if (!basic_program.is_valid()) {
        std::cerr << "Demo::on_create: basic program is invalid." << std::endl;
        return false;
    }

    model = OBJLoader::load("assets/models/suzanne.obj");
    if (!model) {
        std::cerr << "Demo::on_create: unable to load model." << std::endl;
        return false;
    }
    model->update_world_transform();

    camera = std::make_shared<PerspectiveCamera>();
    camera->rotate_x(-45.0f);
    camera->rotate_y(15.0f);
    camera->translate_z(40.0f);
    camera->update_world_transform();

    arcball = std::make_shared<Arcball>(model, camera, window);
    arcball->allow_constraints = true;

    helper.target(arcball);
    helper.show_result = false;

    axes.mode = DrawMode::LINES;
    axes.opacity = 0.3f;
    axes.color = glm::vec3(0.5f, 0.5f, 1.0f);
    axes.use_color = true;
    axes.positions.push_back(glm::vec3(-50.0f, 0.0f, 0.0f)); // x
    axes.positions.push_back(glm::vec3(50.0f, 0.0f, 0.0f));
    axes.positions.push_back(glm::vec3(0.0f, -50.0f, 0.0f)); // y
    axes.positions.push_back(glm::vec3(0.0f, 50.0f, 0.0f));
    axes.positions.push_back(glm::vec3(0.0f, 0.0f, -50.0f)); // z
    axes.positions.push_back(glm::vec3(0.0f, 0.0f, 50.0f));
    axes.update_positions = true;

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

void Demo::update(seconds, seconds, Input & input)
{
    if (light_program.is_modified()) {
        light_program.reload();
    }

    if (basic_program.is_modified()) {
        basic_program.reload();
    }

    if (input.pressed(Key::F1)) {
        show_helpers = !show_helpers;
    }

    arcball->update(input);
    helper.update();

    model->update_world_transform();

    auto dimension = window->dimension();
    if (width != dimension.first || height != dimension.second) {
        width = dimension.first;
        height = dimension.second;

        glViewport(0, 0, width, height);

        camera->aspect_ratio = width / static_cast<float>(height);
    }
}

void Demo::render()
{
    render_model();

    if (show_helpers) {
        render_axes();
        render_helper();
    }
}

void Demo::destroy()
{
}

void Demo::render_model()
{
    light_program.use();

    glm::mat4 m = model->world_transform();
    glm::mat4 v = camera->view();
    glm::mat4 p = camera->projection();

    glm::mat4 mv = v * m;
    glm::mat4 mvp = p * mv;
    glm::mat3 nm = glm::inverseTranspose(glm::mat3(mv));

    glm::vec4 light_direction_es = v * light_direction;

    light_program.set_uniform("mv", mv);
    light_program.set_uniform("mvp", mvp);
    light_program.set_uniform("nm", nm);
    light_program.set_uniform("light_position", light_direction_es);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    model->traverse([&](WorldObject & object) {
        object.draw();
    });
}

void Demo::render_axes()
{
    basic_program.use();

    glm::mat4 v = camera->view();
    glm::mat4 p = camera->projection();
    glm::mat4 mvp = p * v;

    basic_program.set_uniform("mvp", mvp);
    basic_program.set_uniform("use_mesh_color", axes.use_color);
    basic_program.set_uniform("mesh_color", axes.color);
    basic_program.set_uniform("mesh_opacity", axes.opacity);

    axes.draw();
}

void Demo::render_helper()
{
    helper.draw(basic_program, helper_camera);
}
