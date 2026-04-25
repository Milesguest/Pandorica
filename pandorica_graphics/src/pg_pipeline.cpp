#include "../include/pg_pipeline.hpp"
#include "../include/pg.hpp"

namespace pg {

idmap<Pipeline>* pipelines;

Pipeline::Pipeline(id_t id, const SDL_GPUGraphicsPipelineCreateInfo& info) : EngineResource(id) {
    this->pipeline   = SDL_CreateGPUGraphicsPipeline(gpu->device, &info);
    (*pipelines)[id] = this;
}
Pipeline::~Pipeline() {
    SDL_ReleaseGPUGraphicsPipeline(gpu->device, this->pipeline);
    pipelines->erase(id);
}

bool CreatePipeline(const id_t pipelineId, SDL_GPUGraphicsPipelineCreateInfo info, const id_t vertexShaderId, const id_t fragmentShaderId) {
    if (!pipelineId) {
        Log(1, std::format("PipelineId is not allowed to be 0"));
        return 0;
    }

    if (pipelines->contains(pipelineId)) {
        Log(1, std::format("Pipeline at id {} already exists, deleting it", pipelineId));
        delete (*pipelines)[pipelineId];
    }

    if (!shaders->contains(vertexShaderId)) {
        Log(1, std::format("Shader at id {} does not exist, cannot create pipeline", vertexShaderId));
        return 0;
    }

    if (!shaders->contains(fragmentShaderId)) {
        Log(1, std::format("Shader at id {} does not exist, cannot create pipeline", fragmentShaderId));
        return 0;
    }

    info.vertex_shader   = shaders->at(vertexShaderId)->shader;
    info.fragment_shader = shaders->at(fragmentShaderId)->shader;

    if (!(new Pipeline(pipelineId, info))->pipeline) {
        Log(1, SDL_GetError());
        return 0;
    }

    Log(0, std::format("Successfully created pipeline at id {}", pipelineId));
    return 1;
}

bool DeletePipeline(const id_t pipelineId) {
    if (!pipelines->contains(pipelineId)) {
        Log(1, std::format("Pipeline at id {} does not exist, exiting delete function", pipelineId));
        return 0;
    }

    delete (*pipelines)[pipelineId];

    Log(0, std::format("Successfully deleted pipeline at id {}", pipelineId));
    return 1;
}

Pipeline* GetPipeline(const id_t pipelineId) {
    if (!pipelines->contains(pipelineId)) {
        Log(1, std::format("Pipeline at id {} does not exist, cannot get it", pipelineId));
        return nullptr;
    }

    return (*pipelines)[pipelineId];
}

} // namespace pg
