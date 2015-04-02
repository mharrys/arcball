#ifndef DEMO_HPP_INCLUDED
#define DEMO_HPP_INCLUDED

#include "arcball.hpp"
#include "arcballhelper.hpp"
#include "assets.hpp"

#include "gust.hpp"

class Demo : public gst::World {
public:
    Demo(std::shared_ptr<gst::Logger> logger, std::shared_ptr<gst::Window> window);
    bool create() final;
    void update(float delta, float elapsed) final;
    void destroy() final;
private:
    void create_scene();
    void create_arcball();
    void create_lights();
    void update_input();

    std::shared_ptr<gst::Logger> logger;
    std::shared_ptr<gst::Window> window;

    gst::Renderer renderer;
    gst::Scene scene;
    std::shared_ptr<gst::CameraNode> eye;

    gst::Resolution render_size;
    gst::ProgramPool programs;

    Arcball arcball;
    ArcballHelper arcball_helper;

    bool show_helpers;
};

#endif
