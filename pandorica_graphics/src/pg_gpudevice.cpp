#include "../include/pg_gpudevice.hpp"
#include "../include/pg.hpp"

namespace pg {

GPUDevice* gpu;

GPUDevice::~GPUDevice() {
    while (!this->textures.empty())
        delete textures.begin()->second;

    while (!this->buffers.empty())
        delete buffers.begin()->second;

    while (!this->transferBuffers.empty())
        delete transferBuffers.begin()->second;

    while (!this->pipelines.empty())
        delete pipelines.begin()->second;

    while (!this->shaders.empty())
        delete shaders.begin()->second;

    while (!this->samplers.empty())
        delete samplers.begin()->second;
};

bool AcquireCommandBuffer() {
    if (gpu->cCmdBuffer) {
        Log(1, "Already began an cmdBuffer, submitting the old one");
        SubmitCommandBuffer();
    }

    gpu->cCmdBuffer = SDL_AcquireGPUCommandBuffer(gpu->device);
    if (!gpu->cCmdBuffer) {
        Log(2, std::format("Could not acquire a commandBuffer: {}", SDL_GetError()));
        gpu->cCmdBuffer = nullptr;
        return 0;
    }

    Log(0, "Successfully began commandBuffer");
    return 1;
}

bool SubmitCommandBuffer() {
    if (!gpu->cCmdBuffer) {
        Log(1, "Never began a commandBuffer, cannot submit it");
        return 0;
    }

    if (!SDL_SubmitGPUCommandBuffer(gpu->cCmdBuffer)) {
        Log(1, std::format("Could not submit commandBuffer: {}", SDL_GetError()));
        gpu->cCmdBuffer = nullptr;
        return 0;
    }

    gpu->cCmdBuffer = nullptr;

    Log(0, "Successfully submitted commandBuffer");
    return 1;
}

} // namespace pg
