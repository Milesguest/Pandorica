#pragma once
#include "pg_generic.hpp"

/** @file */
namespace pg {

/**
 * @brief A sampler for textures.
 *
 * A sampler is used to manipulate textures with pre-defined functions. They are partially modular.
 *
 * A sampler is part of the GPUDevice.
 *
 * @see GPUDevice, CreateSampler(), DeleteSampler(), GetSampler()
 *
 * @ingroup pg_gpu
 */
struct Sampler : EngineResource {
    SDL_GPUSampler* sampler;

    Sampler(id_t id, const SDL_GPUSamplerCreateInfo& info);
    ~Sampler();
};
extern idmap<Sampler>* samplers; /**< Pointer to 'gpu->samplers'. */

/**
 * @brief Create a sampler
 *
 * @param samplerId The id at which to create the sampler.
 * @param info Info for creating the sampler. See SDL3_gpu for more information.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Sampler, DeleteSampler()
 *
 * @ingroup pg_gpu
 */
bool CreateSampler(const id_t samplerId, const SDL_GPUSamplerCreateInfo& info);
/**
 * @brief Delete a sampler.
 *
 * @param samplerId The id of the sampler to delete.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Sampler, CreateSampler()
 *
 * @ingroup pg_gpu
 */
bool DeleteSampler(const id_t samplerId);
/**
 * @brief Get a sampler
 *
 * @param samplerId The id of the sampler to return.
 * @return A pointer to the sampler on success or NULL on failure. See the logs for more information.
 *
 * @see Sampler
 *
 * @ingroup pg_gpu
 */
Sampler* GetSampler(const id_t samplerId);

} // namespace pg
