#include "demo.hpp"

Demo::Demo(std::shared_ptr<gst::Logger> logger, gst::Window window)
    : logger(logger),
      window(window),
      show_helpers(true)
{
}

bool Demo::create()
{
    const auto size = window.get_size();

    render_state = std::make_shared<gst::RenderState>(size);
    renderer = gst::Renderer(logger, render_state);
    renderer.auto_clear = false;

    auto aspect_ratio = size.get_width() / static_cast<float>(size.get_height());
    auto camera = std::make_shared<gst::PerspectiveCamera>(
        45.0f,
        aspect_ratio,
        0.1f,
        1000.0f
    );
    scene = gst::Scene(camera);
    scene.eye->rotate_x(-30.0f);
    scene.eye->rotate_y(20.0f);
    scene.eye->translate_z(4.2f);

    gst::ProgramFactory program_factory(logger);
    gst::ProgramPool programs(program_factory);
    gst::SurfacePool surfaces(programs);

    create_arcball(surfaces);
    create_axes(surfaces);
    create_lights();

    return true;
}

void Demo::update(gst::seconds, gst::seconds)
{
    auto input = window.get_input();

    if (input.pressed(gst::Key::F1)) {
        show_helpers = !show_helpers;
    }

    arcball->update(input);
    scene.update();

    renderer.clear();
    renderer.render(scene);
    if (show_helpers) {
        arcball_helper->update();
        arcball_helper->draw(renderer);
    }
    renderer.check_errors();
}

void Demo::create_arcball(gst::SurfacePool & surfaces)
{
    gst::MeshFactory mesh_factory(logger, render_state);

    auto blinn_phong = surfaces.create_blinn_phong(BLINNPHONG_VS, BLINNPHONG_FS);
    blinn_phong.cull_face = gst::CullFace::BACK;
    blinn_phong.depth_test = true;
    blinn_phong.material.specular = glm::vec3(1.0f);
    blinn_phong.material.shininess = 21.0f;

    auto suzanne = std::make_shared<gst::GroupNode>();
    for (auto mesh : mesh_factory.create_from_file(MODEL)) {
        auto model = std::make_shared<gst::Model>(mesh, blinn_phong);
        auto model_node = std::make_shared<gst::ModelNode>(model);
        suzanne->add(model_node);
    }

    arcball = std::make_shared<Arcball>(suzanne, scene.eye, window.get_size());
    arcball->allow_constraints = true;
    arcball_helper = std::make_shared<ArcballHelper>(surfaces, render_state);
    arcball_helper->target(arcball);
    arcball_helper->show_result = false;

    scene.add(suzanne);
}

void Demo::create_axes(gst::SurfacePool & surfaces)
{
    auto basic_surface = surfaces.create_basic(BASIC_VS, BASIC_FS);
    basic_surface.material.diffuse = glm::vec3(0.5f, 0.5f, 1.0f);
    basic_surface.depth_test = true;
    basic_surface.opacity = 0.3f;
    basic_surface.blend_mode = gst::BlendMode::INTERPOLATIVE;
    basic_surface.receive_light = false;

    gst::VertexArray vertex_array(render_state);
    gst::Mesh mesh(vertex_array);
    mesh.mode = gst::DrawMode::LINES;
    mesh.positions = {
        render_state,
        { gst::AttribIndex::POSITION, 3, gst::DataType::FLOAT }
    };
    mesh.positions.data = {
        // x
        glm::vec3(-50.0f, 0.0f, 0.0f),
        glm::vec3(50.0f, 0.0f, 0.0f),
        // y
        glm::vec3(0.0f, -50.0f, 0.0f),
        glm::vec3(0.0f, 50.0f, 0.0f),
        // z
        glm::vec3(0.0f, 0.0f, -50.0f),
        glm::vec3(0.0f, 0.0f, 50.0f),
    };
    mesh.positions.buffer_data();
    vertex_array.set(mesh.positions);

    auto model = std::make_shared<gst::Model>(mesh, basic_surface);
    scene.add(std::make_shared<gst::ModelNode>(model));
}

void Demo::create_lights()
{
    const gst::Attenuation attenuation(1.0f, 0.5f, 0.03f);

    auto light0 = std::make_shared<gst::PointLight>();
    light0->diffuse = glm::vec3(0.0f, 0.0f, 1.0f);
    light0->attenuation = attenuation;

    auto light1 = std::make_shared<gst::PointLight>();
    light1->diffuse = glm::vec3(1.0f, 0.0f, 0.0f);
    light1->attenuation = attenuation;

    auto light_node0 = std::make_shared<gst::LightNode>(light0);
    light_node0->position = glm::vec3(1.8f, 1.2f, 1.0f);

    auto light_node1 = std::make_shared<gst::LightNode>(light1);
    light_node1->position = glm::vec3(-2.0f, 1.2f, 2.0f);

    scene.add(light_node0);
    scene.add(light_node1);
}
