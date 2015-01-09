#include "demo.hpp"
#include "eventloop.hpp"
#include "stdoutlogger.hpp"
#include "window.hpp"
#include "windowbuilder.hpp"

int main()
{
    auto logger = std::make_shared<gst::StdoutLogger>();

    gst::WindowBuilder builder(logger);
    builder.set_title("Arcball");

    auto window = builder.build();
    auto loop = gst::EventLoop();
    auto demo = Demo(logger, window);

    return loop.control(window, demo);
}
