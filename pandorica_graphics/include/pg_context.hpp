#pragma once
#include "pg_generic.hpp"
#include "pg_gpudevice.hpp"
#include "pg_textdevice.hpp"
#include "pg_window.hpp"

/** @file */
namespace pg {

/**
 * @brief A struct grouping all objects of a programm.
 *
 * The context of the entire graphical side of the programm.
 *
 * @see Init(), Quit(), GPUDevice, Window, TextDevice, AudioDevice
 *
 * @ingroup pg_main
 */
struct Context {
    GPUDevice*   gpu         = nullptr; /**< A pointer to the @ref GPUDevice. */
    Window*      window      = nullptr; /**< A pointer to the @ref Window. */
    TextDevice*  textDevice  = nullptr; /**< A pointer tho the @ref TextDevice. */
    AudioDevice* audioDevice = nullptr; /**< A pointer to the @ref AudioDevice. */

    bool running; /**< Whether or not the context is currently running (@ref StartLoop()). */

    std::ofstream logFile;  /**< The file to log to. */
    std::string   basePath; /**< The basePath of the programm. It is safe to modify this, but beware, the paths are all relative. */

    Context();
    ~Context();

    void (*linkToPlugin)() = nullptr; /**< Use this to link a function which is called inside StartLoop() before mainFunction(). */
};
extern Context* cContext; /**< The current context of the programm. */

} // namespace pg
