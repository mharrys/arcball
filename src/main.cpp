#include "demo.hpp"
#include "highresolutionclock.hpp"
#include "stdoutlogger.hpp"
#include "windowimpl.hpp"
#include "worldrunner.hpp"

int main()
{
    auto logger = std::make_shared<gst::StdoutLogger>();
    auto window = std::make_shared<gst::WindowImpl>(
        logger,
        // exit on close
        true,
        // exit on esc
        true,
        // fullscreen
        false,
        // resize
        false,
        // size
        gst::Resolution(800, 600),
        // title
        "Arcball"
    );

    if (window->open()) {
        auto runner = gst::WorldRunner();
        auto clock = gst::HighResolutionClock();
        auto demo = Demo(logger, window);
        return runner.control(demo, clock, *window);
    } else {
        return 1;
    }
}
