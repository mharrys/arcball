#ifndef WINDOW_HPP_INCLUDED
#define WINDOW_HPP_INCLUDED

#include "input.hpp"

#include <memory>
#include <string>
#include <utility>

struct NativeWindow;

struct WindowSetting {
    std::string title;
    int width;
    int height;
    bool resize;
    bool fullscreen;
    bool exit_on_close;
    bool exit_on_esc;
};

class Window {
    friend class Runner;
public:
    Window(WindowSetting setting);
    ~Window();

    void request_pointer_lock(bool pointer_lock);

    bool is_open() const;
    bool should_close() const;

    std::pair<int, int> dimension() const;
private:
    bool open();
    void close();

    void poll(Input & input);
    void swap();

    void reset();

    std::unique_ptr<NativeWindow> native;

    bool exit_flag;
    bool pointer_lock;

    WindowSetting setting;
};

#endif
