#pragma once
#include "pg_context.hpp"
#include "pg_generic.hpp"

/** @file */
namespace pg {

/**
 * @brief Initializes Pandorica Graphics.
 *
 * This function initializes every required SDL3 system and also creates a context.
 *
 * @see Context, Quit(), CreateWindowAndGPU()
 *
 * @ingroup pg_main
 */
bool Init();
/**
 * @brief Quits Pandorica Graphics.
 *
 * This function destroys everything inside the current context and the context itself.
 * Every SDL3 system will be deinitialized.
 *
 * @see Context, Init()
 *
 * @ingroup pg_main
 */
void Quit();
/**
 * @brief Logs a message.
 *
 * This function logs a message.
 * If no window was created or the loop has not started, frame count will be invalid.
 * You have to run @ref Init() before this to work.
 *
 * @param level The priority level of the message.
 * 0 = info,
 * 1 = warning,
 * 2 = error.
 * If 2 is selected, the programm will quit.
 * @param message The message to display.
 *
 * @see LOGFILE_NAME
 *
 * @ingroup pg_main
 */
void Log(const int level, const std::string& message);
/**
 * @brief Creates a window and GPUDevice.
 *
 * This function creates a @ref Window and @ref GPUDevice.
 * It should be called after @ref Init()
 *
 * @param title The title of the window.
 * @param width The width of the window.
 * @param height The height of the window.
 * @param flags Various flags for the window. See SDL3 for more information.
 * @param debug Whether or not debug messages should be displayed by SDL3. For releases, this should be false.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @note the width and height is not always constant, depending on the flags.
 *
 * @see Window, GPUDevice
 *
 * @ingroup pg_main
 */
bool CreateWindowAndGPU(const std::string& title, const int width, const int height, const SDL_WindowFlags flags, const bool debug);
/**
 * @brief Creates a standard context.
 *
 * This function creates various engine resources: \n
 * `textures+0   : The texture used as the main color target.
 * textures+1   : The depth texture.
 * textures+2   : The moments texture (used for MBOIT).
 * textures+3   : The absorbance texture (used for MBOIT).
 * textures+4   : The resolved texture (used for MBOIT).
 * shaders+0    : The standard vertex shader ("standard.vert").
 * shaders+1    : The shader used for opaque pixels ("opaque.frag").
 * shaders+2    : The shader used for accumulating moments and absorbance ("accumulation.frag").
 * shaders+3    : The shader used for resolving moments and absorbance ("resolve.frag").
 * shaders+4    : The vertex shader used for drawing the opaque and resolved textures to the screen ("composite.vert").
 * shaders+5    : The fragment shader used for drawing the opaque and resolved textures to the screen ("composite.frag").
 * pipelines+0  : The pipeline used for drawing opaque pixels.
 * pipelines+1  : The pipeline used for accumulation of moments and absorbance.
 * pipelines+2  : The pipeline used for resolving moments and absorbance.
 * pipelines+3  : The pipeline merging the transparent and opaque textures.
 * buffers+0    : The buffer used for storing the 4 standard vertices.
 * buffers+1    : The buffer used for storing the 6 standard indices.
 * transferBuffers+0    : The default texture transferBuffer.
 * transferBuffers+1    : The default data transferBuffer.
 * samplers+0   : The default texture sampler.`
 * (resource+value = @ref START_SYSTEM_RESOURCES + offset)
 *
 * It can be used to get an implementation of MBOIT with various other data, so you do not have to do this yourself.
 *
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @ingroup pg_main
 */
bool CreateStandardContext();
/**
 * @brief Start the rendering loop.
 *
 * This starts the rendering loop.
 * Inside it @c frameCount and @c deltaTime for the window will be set and @c running for the context.
 *
 * @note @ref Render() is not called inside the loop.
 *
 * @param mainFunction A function pointer to your main function. Will be called every frame.
 * @param frameRate The framerate to be used.
 * @note If @ref VSYNC is enabled, the framerate cannot be higher than what V-sync uses.
 *
 * @ingroup pg_main
 */
void StartLoop(void (*mainFunction)(), const uint16_t frameRate);
/**
 * @brief Renders the specified camera.
 *
 * This function has 4 render passes: \n
 * `1 - Opaque Pass
 * 2 - Accumulation Pass
 * 3 - Resolve Pass
 * 4 - Composite Pass`
 *
 * @note This function should only be used if you called @ref CreateStandardContext(). If you did not call it, you will have to create your own rendering function.
 *
 * @param cameraId The id of the camera to render.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @ingroup pg_main
 */
bool Render(const id_t cameraId);

}; // namespace pg
