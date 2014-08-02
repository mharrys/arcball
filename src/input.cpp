#include "input.hpp"

#include <iostream>

Input::Input()
{
    for (unsigned int i = 0; i < mouse.buttons.size(); i++) {
        mouse.buttons[i] = State::UP;
    }

    mouse.scroll_delta = 0;

    for (unsigned int i = 0; i < keyboard.keys.size(); i++) {
        keyboard.keys[i] = State::UP;
    }
}

bool Input::pressed(Key key)
{
    return state(key) == State::PRESSED;
}

bool Input::down(Key key)
{
    return state(key) == State::DOWN;
}

bool Input::released(Key key)
{
    return state(key) == State::RELEASED;
}

bool Input::up(Key key)
{
    return state(key) == State::UP;
}

bool Input::clicked(Button button)
{
    return state(button) == State::CLICKED;
}

bool Input::dblclicked(Button button)
{
    return state(button) == State::DBLCLICKED;
}

bool Input::down(Button button)
{
    return state(button) == State::DOWN;
}

bool Input::released(Button button)
{
    return state(button) == State::RELEASED;
}

bool Input::up(Button button)
{
    return state(button) == State::UP;
}

glm::ivec2 Input::position()
{
    return mouse.position;
}

glm::ivec2 Input::position_rel()
{
    return mouse.position_rel;
}

int Input::scroll_delta()
{
    return mouse.scroll_delta;
}

State Input::state(Button button)
{
    return mouse.buttons[static_cast<int>(button)];
}

State Input::state(Key key)
{
    return keyboard.keys[static_cast<int>(key)];
}

void Input::update()
{
    for (unsigned int i = 0; i < mouse.buttons.size(); i++) {
        State state = mouse.buttons[i];

        if (state == State::CLICKED || state == State::DBLCLICKED) {
            mouse.buttons[i] = State::DOWN;
        } else if (state == State::RELEASED) {
            mouse.buttons[i] = State::UP;
        }
    }

    mouse.scroll_delta = 0;

    for (unsigned int i = 0; i < keyboard.keys.size(); i++) {
        State state = keyboard.keys[i];

        if (state == State::PRESSED) {
            keyboard.keys[i] = State::DOWN;
        } else if (state == State::RELEASED) {
            keyboard.keys[i] = State::UP;
        }
    }
}

void Input::set_state(Button button, State state)
{
    mouse.buttons[static_cast<int>(button)] = state;
}

void Input::set_state(Key key, State state)
{
    keyboard.keys[static_cast<int>(key)] = state;
}
