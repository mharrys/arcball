#include "window.hpp"

#include "lib/gl.hpp"
#include "lib/sdl.hpp"

#include <iostream>

Window::Window(WindowSetting setting)
    : native(new NativeWindow()),
      setting(setting)
{
    reset();
    open();
}

Window::~Window()
{
    if (is_open()) {
        close();
    }
}

bool Window::open()
{
    if (is_open()) {
        std::cerr << "Window::open: attempted to open already open window." << std::endl;
        return false;
    }

    GLenum glew_error;

    int mask = SDL_WINDOW_OPENGL;

    if (setting.resize) {
        mask |= SDL_WINDOW_RESIZABLE;
    }

    if (setting.fullscreen) {
        mask |= SDL_WINDOW_FULLSCREEN;
    }

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
        std::cerr << "Window::open: unable to initialize SDL: " << SDL_GetError() << std::endl;
        goto err_init;
    }

    native->window = SDL_CreateWindow(
        setting.title.c_str(),
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, // respect the window manager
        setting.width,
        setting.height,
        mask
    );
    if (!native->window) {
        std::cerr << "Window::open: could not create native window: " << SDL_GetError() << std::endl;
        goto err_window;
    }

    native->context = SDL_GL_CreateContext(native->window);
    if (!native->context) {
        std::cerr << "Window::open: could not create native window context: " << SDL_GetError() << std::endl;
        goto err_context;
    }

    glew_error = glewInit();
    if (glew_error != GLEW_OK) {
        std::cerr << "Window::open: could not initialize glew: " << glewGetErrorString(glew_error) << std::endl;
        goto err_glew;
    }

    if (!GLEW_VERSION_3_0) {
        std::cerr << "Window::open: OpenGL 3.0 API support is required." << std::endl;
        goto err_glew;
    }

    return true;

err_glew:
    SDL_GL_DeleteContext(native->context);
err_context:
    SDL_DestroyWindow(native->window);
err_window:
    SDL_Quit();
err_init:
    reset();
    return false;
}

void Window::close()
{
    if (!is_open()) {
        std::cerr << "Window::close: attempted to close already closed window." << std::endl;
    } else {
        SDL_GL_DeleteContext(native->context);
        SDL_DestroyWindow(native->window);
        SDL_Quit();
        reset();
    }
}

static Button translate_button(int button)
{
    // Logitech G400 generates 8 and 9 for X1 and X2 in GNU/Linux
    const int G400_X1 = 8;
    const int G400_X2 = 9;

    switch (button) {
    case SDL_BUTTON_LEFT:
        return Button::LEFT;
    case SDL_BUTTON_MIDDLE:
        return Button::MIDDLE;
    case SDL_BUTTON_RIGHT:
        return Button::RIGHT;
    case SDL_BUTTON_X1:
    case G400_X1:
        return Button::X1;
    case SDL_BUTTON_X2:
    case G400_X2:
        return Button::X2;
    default:
        return Button::UNKNOWN;
    }
}

static Key translate_key(int key)
{
    switch (key) {
    case SDLK_BACKSPACE:
        return Key::BACKSPACE;
    case SDLK_TAB:
        return Key::TAB;
    case SDLK_RETURN:
        return Key::RETURN;
    case SDLK_ESCAPE:
        return Key::ESCAPE;
    case SDLK_SPACE:
        return Key::SPACE;
    case SDLK_EXCLAIM:
        return Key::EXCLAMATION;
    case SDLK_QUOTEDBL:
        return Key::QUOTATION;
    case SDLK_HASH:
        return Key::NUMBER;
    case SDLK_DOLLAR:
        return Key::DOLLAR;
    case SDLK_PERCENT:
        return Key::PERCENT;
    case SDLK_AMPERSAND:
        return Key::AMPERSAND;
    case SDLK_QUOTE:
        return Key::APOSTROPHE;
    case SDLK_LEFTPAREN:
        return Key::LBRACKET;
    case SDLK_RIGHTPAREN:
        return Key::RBRACKET;
    case SDLK_ASTERISK:
        return Key::ASTERISK;
    case SDLK_PLUS:
        return Key::PLUS;
    case SDLK_COMMA:
        return Key::COMMA;
    case SDLK_MINUS:
        return Key::MINUS;
    case SDLK_PERIOD:
        return Key::DOT;
    case SDLK_SLASH:
        return Key::SLASH;
    case SDLK_0:
        return Key::D0;
    case SDLK_1:
        return Key::D1;
    case SDLK_2:
        return Key::D2;
    case SDLK_3:
        return Key::D3;
    case SDLK_4:
        return Key::D4;
    case SDLK_5:
        return Key::D5;
    case SDLK_6:
        return Key::D6;
    case SDLK_7:
        return Key::D7;
    case SDLK_8:
        return Key::D8;
    case SDLK_9:
        return Key::D9;
    case SDLK_COLON:
        return Key::COLON;
    case SDLK_SEMICOLON:
        return Key::SEMICOLON;
    case SDLK_LESS:
        return Key::LESS;
    case SDLK_EQUALS:
        return Key::EQUAL;
    case SDLK_GREATER:
        return Key::GREATER;
    case SDLK_QUESTION:
        return Key::QUESTION;
    case SDLK_AT:
        return Key::AT;
    case SDLK_LEFTBRACKET:
        return Key::LSQUARE_BRACKET;
    case SDLK_BACKSLASH:
        return Key::BACKSLASH;
    case SDLK_RIGHTBRACKET:
        return Key::RSQUARE_BRACKET;
    case SDLK_CARET:
        return Key::CARET;
    case SDLK_UNDERSCORE:
        return Key::UNDERSCORE;
    case SDLK_BACKQUOTE:
        return Key::GRAVE_ACCENT;
    case SDLK_a:
        return Key::A;
    case SDLK_b:
        return Key::B;
    case SDLK_c:
        return Key::C;
    case SDLK_d:
        return Key::D;
    case SDLK_e:
        return Key::E;
    case SDLK_f:
        return Key::F;
    case SDLK_g:
        return Key::G;
    case SDLK_h:
        return Key::H;
    case SDLK_i:
        return Key::I;
    case SDLK_j:
        return Key::J;
    case SDLK_k:
        return Key::K;
    case SDLK_l:
        return Key::L;
    case SDLK_m:
        return Key::M;
    case SDLK_n:
        return Key::N;
    case SDLK_o:
        return Key::O;
    case SDLK_p:
        return Key::P;
    case SDLK_q:
        return Key::Q;
    case SDLK_r:
        return Key::R;
    case SDLK_s:
        return Key::S;
    case SDLK_t:
        return Key::T;
    case SDLK_u:
        return Key::U;
    case SDLK_v:
        return Key::V;
    case SDLK_w:
        return Key::W;
    case SDLK_x:
        return Key::X;
    case SDLK_y:
        return Key::Y;
    case SDLK_z:
        return Key::Z;
    case SDLK_KP_LEFTBRACE:
        return Key::LBRACE;
    case SDLK_KP_VERTICALBAR:
        return Key::BAR;
    case SDLK_KP_RIGHTBRACE:
        return Key::RBRACE;
    case SDLK_LALT:
        return Key::LALT;
    case SDLK_RALT:
        return Key::RALT;
    case SDLK_LCTRL:
        return Key::LCTRL;
    case SDLK_RCTRL:
        return Key::RCTRL;
    case SDLK_LSHIFT:
        return Key::LSHIFT;
    case SDLK_RSHIFT:
        return Key::RSHIFT;
    case SDLK_LGUI:
        return Key::LSUPER;
    case SDLK_RGUI:
        return Key::RSUPER;
    case SDLK_MENU:
        return Key::MENU;
    case SDLK_CAPSLOCK:
        return Key::CAPS_LOCK;
    case SDLK_NUMLOCKCLEAR:
        return Key::NUM_LOCK;
    case SDLK_SCROLLLOCK:
        return Key::SCROLL_LOCK;
    case SDLK_PRINTSCREEN:
        return Key::PRINT_SCREEN;
    case SDLK_SYSREQ:
        return Key::SYS_REQ;
    case SDLK_PAUSE:
        return Key::PAUSE;
    case SDLK_CLEAR:
        return Key::CLEAR;
    case SDLK_INSERT:
        return Key::INSERT;
    case SDLK_DELETE:
        return Key::DELETE;
    case SDLK_HOME:
        return Key::HOME;
    case SDLK_END:
        return Key::END;
    case SDLK_PAGEUP:
        return Key::PAGE_UP;
    case SDLK_PAGEDOWN:
        return Key::PAGE_DOWN;
    case SDLK_UP:
        return Key::UP;
    case SDLK_DOWN:
        return Key::DOWN;
    case SDLK_LEFT:
        return Key::LEFT;
    case SDLK_RIGHT:
        return Key::RIGHT;
    case SDLK_F1:
        return Key::F1;
    case SDLK_F2:
        return Key::F2;
    case SDLK_F3:
        return Key::F3;
    case SDLK_F4:
        return Key::F4;
    case SDLK_F5:
        return Key::F5;
    case SDLK_F6:
        return Key::F6;
    case SDLK_F7:
        return Key::F7;
    case SDLK_F8:
        return Key::F8;
    case SDLK_F9:
        return Key::F9;
    case SDLK_F10:
        return Key::F10;
    case SDLK_F11:
        return Key::F11;
    case SDLK_F12:
        return Key::F12;
    case SDLK_F13:
        return Key::F13;
    case SDLK_F14:
        return Key::F14;
    case SDLK_F15:
        return Key::F15;
    case SDLK_F16:
        return Key::F16;
    case SDLK_F17:
        return Key::F17;
    case SDLK_F18:
        return Key::F18;
    case SDLK_F19:
        return Key::F19;
    case SDLK_F20:
        return Key::F20;
    case SDLK_F21:
        return Key::F21;
    case SDLK_F22:
        return Key::F22;
    case SDLK_F23:
        return Key::F23;
    case SDLK_F24:
        return Key::F24;
    case SDLK_KP_0:
        return Key::KP_0;
    case SDLK_KP_1:
        return Key::KP_1;
    case SDLK_KP_2:
        return Key::KP_2;
    case SDLK_KP_3:
        return Key::KP_3;
    case SDLK_KP_4:
        return Key::KP_4;
    case SDLK_KP_5:
        return Key::KP_5;
    case SDLK_KP_6:
        return Key::KP_6;
    case SDLK_KP_7:
        return Key::KP_7;
    case SDLK_KP_8:
        return Key::KP_8;
    case SDLK_KP_9:
        return Key::KP_9;
    case SDLK_KP_PLUS:
        return Key::KP_ADD;
    case SDLK_KP_MINUS:
        return Key::KP_SUBTRACT;
    case SDLK_KP_MULTIPLY:
        return Key::KP_MULTIPLY;
    case SDLK_KP_DIVIDE:
        return Key::KP_DIVIDE;
    // purposly translate period and comma as decimal
    case SDLK_KP_PERIOD:
    case SDLK_KP_COMMA:
        return Key::KP_DECIMAL;
    case SDLK_KP_ENTER:
        return Key::KP_ENTER;
    default:
        return Key::UNKNOWN;
    }
}

void Window::poll(Input & input)
{
    input.update();

    SDL_Event event;
    Button button;
    Key key;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            if (setting.exit_on_close) {
                exit_flag = true;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            button = translate_button(event.button.button);
            if (event.button.clicks > 1) {
                input.set_state(button, State::DBLCLICKED);
            } else {
                input.set_state(button, State::CLICKED);
            }
            break;
        case SDL_MOUSEBUTTONUP:
            button = translate_button(event.button.button);
            input.set_state(button, State::RELEASED);
            break;
        case SDL_MOUSEMOTION:
            input.mouse.position.x = event.motion.x;
            input.mouse.position.y = event.motion.y;
            if (pointer_lock) {
                input.mouse.position_rel.x = event.motion.xrel;
                input.mouse.position_rel.y = event.motion.yrel;
            }
            break;
        case SDL_MOUSEWHEEL:
            input.mouse.scroll_delta += event.wheel.y;
            break;
        case SDL_KEYDOWN:
            key = translate_key(event.key.keysym.sym);
            if (event.key.repeat == 0) {
                input.set_state(key, State::PRESSED);
            }
            break;
        case SDL_KEYUP:
            key = translate_key(event.key.keysym.sym);
            input.set_state(key, State::RELEASED);
            if (setting.exit_on_esc && key == Key::ESCAPE) {
                exit_flag = true;
            }
            break;
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                setting.width = event.window.data1;
                setting.height = event.window.data2;
            }
            break;
        }
    }
}

void Window::swap()
{
    if (!is_open()) {
        std::cerr << "Window::swap: attempted to swap buffers on closed window." << std::endl;
    } else {
        SDL_GL_SwapWindow(native->window);
    }
}

void Window::request_pointer_lock(bool pointer_lock)
{
    if (SDL_SetRelativeMouseMode(pointer_lock ? SDL_TRUE : SDL_FALSE) == 0) {
        this->pointer_lock = pointer_lock;
    } else {
        std::cerr << "Window::request_pointer_lock: " << SDL_GetError() << std::endl;
    }
}

bool Window::is_open() const
{
    return native->window != 0;
}

bool Window::should_close() const
{
    return exit_flag;
}

std::pair<int, int> Window::dimension() const
{
    return std::make_pair(setting.width, setting.height);
}

void Window::reset()
{
    native->window = 0;
    native->context = 0;
    exit_flag = false;
    pointer_lock = false;
}
