#include "../include/pg_sampler.hpp"
#include "../include/pg.hpp"

namespace pg {

idmap<Sampler>* samplers;

Sampler::Sampler(id_t id, const SDL_GPUSamplerCreateInfo& info) : EngineResource(id) {
    this->sampler   = SDL_CreateGPUSampler(gpu->device, &info);
    (*samplers)[id] = this;
}
Sampler::~Sampler() {
    SDL_ReleaseGPUSampler(gpu->device, this->sampler);
    samplers->erase(id);
}

bool CreateSampler(const id_t samplerId, const SDL_GPUSamplerCreateInfo& info) {
    if (!samplerId) {
        Log(1, "SamplerId not allowed to be 0");
        return 0;
    }

    if (samplers->contains(samplerId)) {
        Log(1, std::format("Sampler at id {} already exists, deleting it", samplerId));
        delete (*samplers)[samplerId];
    }

    if (!(new Sampler(samplerId, info))->sampler) {
        Log(1, SDL_GetError());
        return 0;
    }

    Log(0, std::format("Successfully created sampler at id {}", samplerId));
    return 1;
}

bool DeleteSampler(const id_t samplerId) {
    if (!samplers->contains(samplerId)) {
        Log(1, std::format("Sampler at id {} does not exist, exiting delete function", samplerId));
        return 0;
    }

    delete (*samplers)[samplerId];

    Log(0, std::format("Successfully deleted sampler at id {}", samplerId));
    return 1;
}

Sampler* GetSampler(const id_t samplerId) {
    if (!samplers->contains(samplerId)) {
        Log(1, std::format("Sampler at id {} does not exist, cannot get it", samplerId));
        return nullptr;
    }

    return (*samplers)[samplerId];
}

} // namespace pg
