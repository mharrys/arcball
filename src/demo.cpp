#include "demo.hpp"

Demo::Demo(std::shared_ptr<gst::Logger> logger, std::shared_ptr<gst::Window> window)
    : logger(logger),
      window(window),
      renderer(gst::Renderer::create(logger)),
      render_size(window->get_size()),
      programs(logger),
      show_helpers(true)
{
}

bool Demo::create()
{
    renderer.set_auto_clear(false, false);
    renderer.set_viewport(render_size);

    create_scene();
    create_arcball();
    create_lights();

    return true;
}

void Demo::update(float, float)
{
    update_input();

    renderer.clear(true, true);
    renderer.render(scene);

    if (show_helpers) {
        arcball_helper.update(arcball);
        auto helpers = arcball_helper.get_helpers();
        renderer.render(helpers);
    }
}

void Demo::destroy()
{
}

void Demo::create_scene()
{
    auto camera = std::unique_ptr<gst::Camera>(new gst::PerspectiveCamera(45.0f, render_size, 0.1f, 1000.0f));
    eye = std::make_shared<gst::CameraNode>(std::move(camera));
    eye->rotate_x(-30.0f);
    eye->rotate_y(20.0f);
    eye->translate_z(4.2f);
    scene = gst::Scene(eye);
}

void Demo::create_arcball()
{
    auto blinn_phong_program = programs.create(BLINNPHONG_VS, BLINNPHONG_FS);
    auto shaded_pass = std::make_shared<gst::ShadedPass>(blinn_phong_program);
    shaded_pass->set_cull_face(gst::CullFace::BACK);
    shaded_pass->set_depth_test(true);

    auto material = gst::Material::create_struct("material");
    material.get_uniform("ambient") = glm::vec3(1.2f);
    material.get_uniform("diffuse") = glm::vec3(0.8f);
    material.get_uniform("specular") = glm::vec3(1.0f);
    material.get_uniform("emission") = glm::vec3(0.0f);
    material.get_uniform("shininess") = 21.0f;

    gst::MeshFactory mesh_factory(logger);
    auto suzanne = std::make_shared<gst::GroupNode>();
    for (auto mesh : mesh_factory.create_from_file(SUZANNE_OBJ)) {
        auto model = gst::Model(mesh, material, shaded_pass);
        auto model_node = std::make_shared<gst::ModelNode>(model);
        suzanne->add(model_node);
    }
    scene.add(suzanne);

    arcball = Arcball(suzanne);
    arcball.set_allow_constraints(true);

    arcball_helper = ArcballHelper::create(programs);
    arcball_helper.set_show_result(false);
}

void Demo::create_lights()
{
    auto create_light = [](glm::vec3 diffuse)
    {
        auto light = gst::Light::create_array("point_lights");
        light.get_uniform("ambient") = glm::vec3(0.0f);
        light.get_uniform("diffuse") = diffuse;
        light.get_uniform("specular") = glm::vec3(1.0f);
        light.get_uniform("attenuation.constant") = 1.0f;
        light.get_uniform("attenuation.linear") = 0.5f;
        light.get_uniform("attenuation.quadratic") = 0.03f;
        return light;
    };

    auto light_node0 = std::make_shared<gst::LightNode>(create_light(glm::vec3(0.0f, 0.0f, 1.0f)));
    light_node0->position = glm::vec3(1.8f, 1.2f, 1.0f);
    scene.add(light_node0);

    auto light_node1 = std::make_shared<gst::LightNode>(create_light(glm::vec3(1.0f, 0.0f, 0.0f)));
    light_node1->position = glm::vec3(-2.0f, 1.2f, 2.0f);
    scene.add(light_node1);
}

void Demo::update_input()
{
    auto input = window->get_input();

    if (input.pressed(gst::Key::F1)) {
        show_helpers = !show_helpers;
    }

    arcball.update(input, scene.get_eye(), render_size);
    scene.update();
}
