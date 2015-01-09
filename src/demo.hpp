#ifndef DEMO_HPP_INCLUDED
#define DEMO_HPP_INCLUDED

#include "arcball.hpp"
#include "arcballhelper.hpp"
#include "assets.hpp"
#include "cameranode.hpp"
#include "effectcomposer.hpp"
#include "effectcomposerfactory.hpp"
#include "firstpersoncontrol.hpp"
#include "groupnode.hpp"
#include "input.hpp"
#include "lightnode.hpp"
#include "logger.hpp"
#include "mesh.hpp"
#include "meshfactory.hpp"
#include "model.hpp"
#include "modelnode.hpp"
#include "orthocamera.hpp"
#include "pass.hpp"
#include "perspectivecamera.hpp"
#include "pointlight.hpp"
#include "renderer.hpp"
#include "renderstate.hpp"
#include "scene.hpp"
#include "surfacepool.hpp"
#include "window.hpp"
#include "world.hpp"

class Demo : public gst::World {
public:
    Demo(std::shared_ptr<gst::Logger> logger, gst::Window window);
    bool create() override;
    void update(gst::seconds delta, gst::seconds elapsed);
private:
    void create_arcball(gst::SurfacePool & surfaces);
    void create_axes(gst::SurfacePool & surfaces);
    void create_lights();

    std::shared_ptr<gst::Logger> logger;
    gst::Window window;

    std::shared_ptr<gst::RenderState> render_state;
    gst::Renderer renderer;
    gst::Scene scene;

    std::shared_ptr<Arcball> arcball;
    std::shared_ptr<ArcballHelper> arcball_helper;

    bool show_helpers;
};

#endif
