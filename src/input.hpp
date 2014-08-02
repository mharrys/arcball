#ifndef INPUT_HPP_INCLUDED
#define INPUT_HPP_INCLUDED

#include "button.hpp"
#include "key.hpp"

#include "lib/glm.hpp"

#include <array>

enum class State {
    UP = 0,
    DOWN,
    PRESSED = 2,
    CLICKED = 2,
    DBLCLICKED,
    RELEASED
};

typedef std::array<State, 6> Buttons;
typedef std::array<State, 139> Keys;

struct Mouse {
    Buttons buttons;
    glm::ivec2 position;
    glm::ivec2 position_rel;
    int scroll_delta;
};

struct Keyboard {
    Keys keys;
};

class Input {
    friend class Window;
public:
    Input();

    bool pressed(Key key);
    bool down(Key key);
    bool released(Key key);
    bool up(Key key);

    bool clicked(Button button);
    bool dblclicked(Button button);
    bool down(Button button);
    bool released(Button button);
    bool up(Button button);

    glm::ivec2 position();
    glm::ivec2 position_rel();
    int scroll_delta();

    State state(Button button);
    State state(Key key);
private:
    void update();

    void set_state(Button button, State state);
    void set_state(Key key, State state);

    Mouse mouse;
    Keyboard keyboard;
};

#endif
