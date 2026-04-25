#pragma once
#include "pg_audiodevice.hpp"
#include "pg_camera.hpp"
#include "pg_generic.hpp"
#include "pg_object.hpp"
#include "pg_text.hpp"
#include "pg_workspace.hpp"

/** @file */
namespace pg {
/**
 * @brief A window used for displaying objects.
 *
 * A window is a window on the display server. To it can be rendered.
 *
 * This struct also holds important information about the loop.
 *
 * @see Object, Camera, Workspace.
 *
 * @ingroup pg_window
 */
struct Window {
    SDL_Window* window; /**< The interface for communication with the window. */

    idmap<Object>    objects{};    /**< A map of all objects. */
    idmap<Camera>    cameras{};    /**< A map of all cameras. */
    idmap<Workspace> workspaces{}; /**< A map of all workspaces. */

    std::queue<SDL_Event> events{}; /**< Queue with events (some events will already be handled). */

    Window(SDL_Window* window);
    ~Window();

    Uint32 width;  /**< The width of the window. */
    Uint32 height; /**< The height of the window. */

    long double deltaTime  = 0; /**< The time between the last two frames. */
    uint64_t    frameCount = 0; /**< The number of frames since the start of the loop. */
    uint64_t    timeMS     = 0; /**< The time in ms since the start of the loop. */
};
extern Window* window; /**< Pointer to the created window. */

} // namespace pg
