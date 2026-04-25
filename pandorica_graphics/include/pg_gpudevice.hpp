#pragma once
#include "pg_buffer.hpp"
#include "pg_generic.hpp"
#include "pg_pipeline.hpp"
#include "pg_sampler.hpp"
#include "pg_shader.hpp"
#include "pg_texture.hpp"
#include "pg_transferbuffer.hpp"

/** @file */
namespace pg {
/**
 * @brief A struct that groups all data of the GPU together and holds a logical gpu device.
 *
 * @see Context, Window, AcquireCommandBuffer(), SubmitCommandBuffer()
 *
 * @ingroup pg_gpu
 */
struct GPUDevice {
    SDL_GPUDevice* device; /**< A pointer to the actual GPUDevice (SDL3_gpu) */

    idmap<Texture>        textures;        /**< A map of all textures. */
    idmap<Buffer>         buffers;         /**< A map of all buffers. */
    idmap<TransferBuffer> transferBuffers; /**< A map of all transferBuffers */
    idmap<Pipeline>       pipelines;       /**< A map of all pipelines. */
    idmap<Shader>         shaders;         /**< A map of all textures. */
    idmap<Sampler>        samplers;        /**< A map of all samplers. */

    /**
     * @brief The current command buffer.
     * @note Will automatically be uploaded when rendering.
     */
    SDL_GPUCommandBuffer* cCmdBuffer = nullptr;

    SDL_GPUTextureFormat depthFormat;   /**< The automatically found compatible depth texture format. */
    SDL_GPUTextureFormat textureFormat; /**< The automatically found compatible texture format. */
    SDL_GPUSampleCount   sampleCount;   /**< The automatically found compatible MS sample count */

    GPUDevice(SDL_GPUDevice* device) : device(device) {};
    ~GPUDevice();
};
extern GPUDevice* gpu; /**< A pointer to the newly created GPUDevice. */

/**
 * @brief Acquires a new commandBuffer and submits the old one.
 *
 * @note commandBuffers are automatically acquired when needed.
 *
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see GPUDevice, SubmitCommandBuffer()
 *
 * @ingroup pg_gpu
 */
bool AcquireCommandBuffer();
/**
 * @brief Submit a commandBuffer.
 *
 * @note commandBuffers are automatically submitted after @ref Render().
 *
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see GPUDevice, AcquireCommandBuffer()
 *
 * @ingroup pg_gpu
 */
bool SubmitCommandBuffer();

} // namespace pg
