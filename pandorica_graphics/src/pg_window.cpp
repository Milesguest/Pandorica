#include "../include/pg_window.hpp"
#include "../include/pg.hpp"

namespace pg {

Window* window;

Window::Window(SDL_Window* window) {
    this->window = window;
}

Window::~Window() {
    while (!this->objects.empty())
        delete objects.begin()->second;

    while (!this->cameras.empty())
        delete cameras.begin()->second;

    while (!this->workspaces.empty())
        delete workspaces.begin()->second;

    SDL_DestroyWindow(window);
};

} // namespace pg
