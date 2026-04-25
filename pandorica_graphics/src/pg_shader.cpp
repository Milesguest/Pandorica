#include "../include/pg_shader.hpp"
#include "../include/pg.hpp"

namespace pg {

idmap<Shader>* shaders;

Shader::Shader(id_t id, const SDL_GPUShaderCreateInfo& info) : EngineResource(id) {
    this->shader   = SDL_CreateGPUShader(gpu->device, &info);
    (*shaders)[id] = this;
}
Shader::~Shader() {
    SDL_ReleaseGPUShader(gpu->device, this->shader);
    (*shaders).erase(id);
}

bool CreateShader(const id_t shaderId, const std::string& path, SDL_GPUShaderCreateInfo info) {
    if (!shaderId) {
        Log(1, "ShaderId not allowed to be 0");
        return 0;
    }

    if (shaders->contains(shaderId)) {
        Log(1, std::format("Shader at id {} already exists, deleting it", shaderId));
        return 0;
    }

    // Altered code from 'https://github.com/TheSpydog/SDL_gpu_examples'.

    if (path.find(".vert") != std::string::npos)
        info.stage = SDL_GPU_SHADERSTAGE_VERTEX;
    else if (path.find(".frag") != std::string::npos)
        info.stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    else
        Log(2, std::format("Could not detect shaderStage of {}", path));

    const Uint32 backendFormats = SDL_GetGPUShaderFormats(gpu->device);

    std::string fullPath;
    if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV) {
        fullPath        = std::format("{}/shaders/spv/{}.spv", cContext->basePath, path);
        info.format     = SDL_GPU_SHADERFORMAT_SPIRV;
        info.entrypoint = "main";
    } else if (backendFormats & SDL_GPU_SHADERFORMAT_MSL) {
        fullPath        = std::format("{}/shaders/msl/{}.msl", cContext->basePath, path);
        info.format     = SDL_GPU_SHADERFORMAT_MSL;
        info.entrypoint = "main0";
    } else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL) {
        fullPath        = std::format("{}/shaders/dxil/{}.dxil", cContext->basePath, path);
        info.format     = SDL_GPU_SHADERFORMAT_DXIL;
        info.entrypoint = "main";
    } else
        Log(2, "No supported shader formats available");

    std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
    if (!file) {
        Log(1, std::format("Could not open shader file {}", fullPath));
        return 0;
    };

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<Uint8> code(size);

    if (!file.read((char*)code.data(), size)) {
        Log(1, std::format("Could not read shader file {}", fullPath));
        return 0;
    }

    info.code      = code.data();
    info.code_size = code.size();

    if (!(new Shader(shaderId, info))->shader) {
        Log(1, SDL_GetError());
        return 0;
    }

    file.close();

    Log(0, std::format("Successfully created shader at id {}", shaderId));

    return 1;
}

bool DeleteShader(const id_t shaderId) {
    if (!shaders->contains(shaderId)) {
        Log(1, std::format("Shader at id {} does not exist, exiting delete function", shaderId));
        return 0;
    }

    delete (*shaders)[shaderId];

    Log(0, std::format("Successfully deleted shader at id {}", shaderId));
    return 1;
}

Shader* GetShader(const id_t shaderId) {
    if (!shaders->contains(shaderId)) {
        Log(1, std::format("Shader at id {} does not exist, cannot get it", shaderId));
        return nullptr;
    }

    return (*shaders)[shaderId];
}

} // namespace pg
