#pragma once
#include "pg_generic.hpp"

/** @file */
namespace pg {

/**
 * @brief A shader used for running code on the gpu.
 *
 * A shader is essentially code that can be run on the GPU.
 * There are many different formats for shaders but the most common are: .dxil (DirectX), .msl (Metall), .spv (Vulkan "SPIR-V").
 * There is a utility programm named 'SDL3_shadercross' to convert them from one to another, however you will have to install this manually.
 * Usually you write shaders here in hlsl but glsl might also work.
 *
 * A shader can be added into the standard context by creating one at the id you wish to replace. For the ids see @ref CreateStandardContext().
 *
 * Shaders are part of the GPUDevice.
 *
 * @see GPUDevice, CreateShader(), DeleteShader(), GetShader()
 *
 * @ingroup pg_gpu
 */
struct Shader : EngineResource {
    SDL_GPUShader* shader; /**< Pointer to the actual shader. */

    Shader(id_t id, const SDL_GPUShaderCreateInfo& info);
    ~Shader();
};
extern idmap<Shader>* shaders; /**< Pointer to 'gpu->shaders'. */

/**
 * @brief Creates a shader.
 *
 * Create a shader at the specified id.
 *
 * It is safe to delete the shader after the graphics pipeline was created.
 *
 * @param shaderId The id at which to create the shader.
 * @param path The path where the shader can be found. It will be assumed the shaders are located in 'BASEPATH/shaders/SHADER_DRIVER/path'.
 * @param info Info with which to create the shader. Do not set format, entrypoint, code and code_size.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Shader, DeleteShader()
 *
 * @ingroup pg_gpu
 */
bool CreateShader(const id_t shaderId, const std::string& path, SDL_GPUShaderCreateInfo info);
/**
 * @brief Deletes a shader.
 *
 * @param shaderId The id of the shader to delete.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Shader, CreateShader()
 *
 * @ingroup pg_gpu
 */
bool DeleteShader(const id_t shaderId);
/**
 * @brief Get a shader.
 *
 * @param shaderId The id of the shader to return.
 * @return The shader on success or NULL on failure. See the logs for more information.
 *
 * @see Shader
 *
 * @ingroup pg_gpu
 */
Shader* GetShader(const id_t shaderId);

} // namespace pg
