#pragma once
#include "pg_generic.hpp"

/** @file */
namespace pg {

/**
 * @brief A struct used for graphic pipelines.
 *
 * This struct holds the graphics pipeline from SDL_gpu.
 *
 * A pipeline essentially is a step-to-step instruction of what the GPU needs to do when rendering.
 *
 * Pipelines are part of the GPUDevice
 *
 * @see GPUDevice, CreatePipeline(), DeletePipeline(), GetPipeline()
 *
 * @ingroup pg_gpu
 */
struct Pipeline : EngineResource {
    SDL_GPUGraphicsPipeline* pipeline; /**< A pointer to the actual graphics pipeline. */

    Pipeline(id_t id, const SDL_GPUGraphicsPipelineCreateInfo& info);
    ~Pipeline();
};
extern idmap<Pipeline>* pipelines; /**< Pointer to 'gpu->pipelines'. */

/**
 * @brief Creates a pipeline.
 *
 * With this function you can create a graphics pipeline.
 *
 * Usually you don't want to create new pipelines, because it is fairly complicated. The only reason you would create one is if you are not using the standard context.
 *
 * @param pipelineId The id of the newly created graphics pipeline.
 * @param info Info about the graphics pipeline.
 * @param vertexShaderId The id of the vertex shader to use.
 * @param fragmentShaderId The id of the fragment shader to use.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Pipeline, Shader, DeletePipeline()
 *
 * @ingroup pg_gpu
 */
bool CreatePipeline(const id_t pipelineId, SDL_GPUGraphicsPipelineCreateInfo info, const id_t vertexShaderId, const id_t fragmentShaderId);
/**
 * @brief Deletes a pipeline
 *
 * @param pipelineId The id of the pipeline to delete.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Pipeline, CreatePipeline()
 *
 * @ingroup pg_gpu
 */
bool DeletePipeline(const id_t pipelineId);
/**
 * @brief Get a pipeline
 *
 * @param pipelineId The id of the pipeline to return.
 * @return A pointer to the pipeline on success, NULL on failure. See the logs for more information.
 *
 * @see Pipeline
 *
 * @ingroup pg_gpu
 */
Pipeline* GetPipeline(const id_t pipelineId);

} // namespace pg
