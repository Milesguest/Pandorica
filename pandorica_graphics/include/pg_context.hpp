#pragma once
#include "pg_generic.hpp"
#include "pg_gpudevice.hpp"
#include "pg_textdevice.hpp"
#include "pg_window.hpp"

/** @file */
namespace pg {

/**
 * @brief A struct grouping all objects of a program.
 *
 * The context of the entire graphical side of the program.
 *
 * @see Init(), Quit(), GPUDevice, Window, TextDevice, AudioDevice
 *
 * @ingroup pg_main
 */
struct Context {
    GPUDevice*   gpu         = nullptr; /**< A pointer to the @ref GPUDevice. */
    Window*      window      = nullptr; /**< A pointer to the @ref Window. */
    TextDevice*  textDevice  = nullptr; /**< A pointer to the @ref TextDevice. */
    AudioDevice* audioDevice = nullptr; /**< A pointer to the @ref AudioDevice. */

    bool running; /**< Whether or not the context is currently running (@ref StartLoop()). */

    std::ofstream logFile;  /**< The file to log to. */
    std::string   basePath; /**< The basePath of the program. It is safe to modify this, but beware, all paths are relative. */

    Context();
    ~Context();

    void (*linkToPlugin)()    = nullptr;       /**< Use this to link a function which is called inside StartLoop() before mainFunction(). */
    void (**lastPluginLink)() = &linkToPlugin; /**< The link of the last plugin that was linked. These functions should be chained together. */
};
extern Context* cContext; /**< The current context of the program. */

/**
 * @brief Links a function that will be called in the loop
 *
 * Allows to link a function from another plugin or file to be used with pandorica graphics
 *
 * @note The new plugin can be a nullptr when trying to call it
 *
 * @param pluginFunction The function you would like to be called inside of the loop.
 * @param newPluginLink The function that you will have to call inside your pluginFunction.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 *
 * @see Context
 *
 * @ingroup pg_main
 */
bool LinkPluginFunction(void (*pluginFunction)(), void (**newPluginLink)());

} // namespace pg
