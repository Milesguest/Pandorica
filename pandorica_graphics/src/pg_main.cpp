#include "../include/pg_main.hpp"
#include "../include/pg_generic.hpp"

namespace pg {

bool Init() {
    SDL_SetHint(SDL_HINT_LOGGING, "app=info, *=verbose");
    cContext = new Context;
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_AUDIO))
        Log(2, std::format("Could not initialize SDL: {}", SDL_GetError()));

    if (!TTF_Init())
        Log(2, std::format("Could not initialize SDL_ttf: {}", SDL_GetError()));

    if (!MIX_Init()) {
        Log(2, std::format("Could not initialize SDL_mixer: {}", SDL_GetError()));
    }

    return 1;
};

void Quit() {
    TTF_Quit();
    SDL_Quit();
    Log(0, "Successfully exited the programm");
    std::flush(cContext->logFile);
    delete cContext;
};

void Log(const int level, const std::string& message) {
    switch (level) {
    case 0:
        if (settings::LOG_LEVEL == 0) {
            if (window)
                cContext->logFile << std::format("({:0>10}) ", window->timeMS % 10000000000);
            cContext->logFile << std::format("[ Info ] {}\n", message);
        }
        return;
    case 1:
        if (settings::LOG_LEVEL <= 1) {
            if (window)
                cContext->logFile << std::format("({:0>10}) ", window->timeMS % 10000000000);
            cContext->logFile << std::format("[ Warn ] {}\n", message);
            std::flush(cContext->logFile);
        }
        return;
    default:
        Log(2, "Selected wrong logging level");
        return;
    case 2:
        std::stringstream cTime;
        time_t            in_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        cTime << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");

        if (window)
            cContext->logFile << std::format("({:0>10}) ", window->timeMS % 10000000000);
        cContext->logFile << std::format("[ Stop ] {}\n", message) << "Error received at " << cTime.str() << '\n';
        std::flush(cContext->logFile);
        Quit();
        std::exit(-1);
        return;
    }
};

bool CreateWindowAndGPU(const std::string& title, const int width, const int height, const SDL_WindowFlags flags, const bool debug) {
    // SDL_SetHint(SDL_HINT_AUDIO_DEVICE_STREAM_NAME, title.c_str());

    if (cContext->window) {
        Log(1, "Window was already created, deleting the old one");
        delete cContext->window;
    }

    cContext->window = new Window(SDL_CreateWindow(title.c_str(), width, height, flags));
    if (!cContext->window->window) {
        Log(2, std::format("Could not create a window: {}", SDL_GetError()));
        return 0;
    }
    window = cContext->window;

    objects    = &window->objects;
    cameras    = &window->cameras;
    workspaces = &window->workspaces;

    if (cContext->gpu) {
        Log(1, "GPUDevice was already created, deleting the old one");
        delete cContext->gpu;
    }

    cContext->gpu = new GPUDevice(SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_MSL, debug, NULL));
    if (!cContext->gpu->device) {
        Log(2, std::format("Could not create a gpu device: {}", SDL_GetError()));
        return 0;
    }
    gpu = cContext->gpu;

    textures        = &gpu->textures;
    buffers         = &gpu->buffers;
    transferBuffers = &gpu->transferBuffers;
    pipelines       = &gpu->pipelines;
    shaders         = &gpu->shaders;
    samplers        = &gpu->samplers;

    if (!SDL_ClaimWindowForGPUDevice(gpu->device, window->window)) {
        Log(2, std::format("Could not claim window for gpu device: {}", SDL_GetError()));
        return 0;
    }

    SDL_GetWindowSizeInPixels(window->window, reinterpret_cast<int*>(&window->width), reinterpret_cast<int*>(&window->height));

    time_t in_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::stringstream cTime;
    cTime << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    cContext->logFile << "=======================================================\n"
                      << "TIME:          " << cTime.str() << '\n'
                      << "GPU:           " << SDL_GetStringProperty(SDL_GetGPUDeviceProperties(gpu->device), SDL_PROP_GPU_DEVICE_NAME_STRING, "Unknown") << '\n'
                      << "DRIVER:        " << SDL_GetGPUDeviceDriver(gpu->device) << '\n'
                      << "RAM:           " << SDL_GetSystemRAM() << "MiB\n"
                      << "LOGICAL CORES: " << SDL_GetNumLogicalCPUCores() << '\n'
                      << "OS:            " << SDL_GetPlatform() << '\n'
                      << "=======================================================\n"
                      << "(   TIME   ) [ TYPE ] MESSAGE\n";
    std::flush(cContext->logFile);

    Log(0, "Successfully created window and gpu device");
    return 1;
}

inline void CreateTextures() {
    CreateTexture(settings::START_SYSTEM_RESOURCES, 0, "",
                  {.type                 = SDL_GPU_TEXTURETYPE_2D,
                   .format               = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
                   .usage                = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_GRAPHICS_STORAGE_READ,
                   .width                = window->width,
                   .height               = window->height,
                   .layer_count_or_depth = 1,
                   .num_levels           = 1,
                   .sample_count         = gpu->sampleCount});

    CreateTexture(settings::START_SYSTEM_RESOURCES + 1, 0, "",
                  {.type                 = SDL_GPU_TEXTURETYPE_2D,
                   .format               = gpu->depthFormat,
                   .usage                = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
                   .width                = window->width,
                   .height               = window->height,
                   .layer_count_or_depth = 1,
                   .num_levels           = 1,
                   .sample_count         = gpu->sampleCount});

    CreateTexture(settings::START_SYSTEM_RESOURCES + 2, 0, "",
                  {.type                 = SDL_GPU_TEXTURETYPE_2D,
                   .format               = SDL_GPU_TEXTUREFORMAT_R32G32B32A32_FLOAT,
                   .usage                = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_GRAPHICS_STORAGE_READ,
                   .width                = window->width,
                   .height               = window->height,
                   .layer_count_or_depth = 1,
                   .num_levels           = 1,
                   .sample_count         = gpu->sampleCount});

    CreateTexture(settings::START_SYSTEM_RESOURCES + 3, 0, "",
                  {.type                 = SDL_GPU_TEXTURETYPE_2D,
                   .format               = SDL_GPU_TEXTUREFORMAT_R32_FLOAT,
                   .usage                = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_GRAPHICS_STORAGE_READ,
                   .width                = window->width,
                   .height               = window->height,
                   .layer_count_or_depth = 1,
                   .num_levels           = 1,
                   .sample_count         = gpu->sampleCount});

    CreateTexture(settings::START_SYSTEM_RESOURCES + 4, 0, "",
                  {.type                 = SDL_GPU_TEXTURETYPE_2D,
                   .format               = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
                   .usage                = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_GRAPHICS_STORAGE_READ,
                   .width                = window->width,
                   .height               = window->height,
                   .layer_count_or_depth = 1,
                   .num_levels           = 1,
                   .sample_count         = gpu->sampleCount});
}

bool CreateStandardContext() {
    // Check if GPU supports...
    if (SDL_GPUTextureSupportsFormat(gpu->device, SDL_GPU_TEXTUREFORMAT_D32_FLOAT, SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET))
        gpu->depthFormat = SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
    else if (SDL_GPUTextureSupportsFormat(gpu->device, SDL_GPU_TEXTUREFORMAT_D24_UNORM, SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET))
        gpu->depthFormat = SDL_GPU_TEXTUREFORMAT_D24_UNORM;
    else
        gpu->depthFormat = SDL_GPU_TEXTUREFORMAT_D16_UNORM;

    if (!SDL_WindowSupportsGPUPresentMode(gpu->device, window->window, static_cast<SDL_GPUPresentMode>(settings::VSYNC)))
        settings::VSYNC = SDL_GPU_PRESENTMODE_VSYNC;

    if (settings::COMPRESSED_TEXTURE_FORMAT >= 0 &&
        SDL_GPUTextureSupportsFormat(gpu->device, static_cast<SDL_GPUTextureFormat>(settings::COMPRESSED_TEXTURE_FORMAT), SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREUSAGE_SAMPLER) &&
        SDL_SetGPUSwapchainParameters(gpu->device, window->window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, static_cast<SDL_GPUPresentMode>(settings::VSYNC)))
        gpu->textureFormat = static_cast<SDL_GPUTextureFormat>(settings::COMPRESSED_TEXTURE_FORMAT);
    else if (SDL_GPUTextureSupportsFormat(gpu->device, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM_SRGB, SDL_GPU_TEXTURETYPE_2D, SDL_GPU_TEXTUREUSAGE_SAMPLER) &&
             SDL_SetGPUSwapchainParameters(gpu->device, window->window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR_LINEAR, static_cast<SDL_GPUPresentMode>(settings::VSYNC))) {
        gpu->textureFormat = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM_SRGB;
        gpu->textureFormat = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
        SDL_SetGPUSwapchainParameters(gpu->device, window->window, SDL_GPU_SWAPCHAINCOMPOSITION_SDR, static_cast<SDL_GPUPresentMode>(settings::VSYNC));
    } else {
    }
    switch (static_cast<int>(settings::DEFAULT_SAMPLE_COUNT)) {
    case 8:
        if (SDL_GPUTextureSupportsSampleCount(gpu->device, gpu->textureFormat, SDL_GPU_SAMPLECOUNT_8)) {
            gpu->sampleCount = SDL_GPU_SAMPLECOUNT_8;
            break;
        }
    case 4:
        if (SDL_GPUTextureSupportsSampleCount(gpu->device, gpu->textureFormat, SDL_GPU_SAMPLECOUNT_4)) {
            gpu->sampleCount = SDL_GPU_SAMPLECOUNT_4;
            break;
        }
    case 2:
        if (SDL_GPUTextureSupportsSampleCount(gpu->device, gpu->textureFormat, SDL_GPU_SAMPLECOUNT_2)) {
            gpu->sampleCount = SDL_GPU_SAMPLECOUNT_2;
            break;
        }
    default:
        gpu->sampleCount = SDL_GPU_SAMPLECOUNT_1;
        break;
    }

    // Create Pipeline and shaders
    if (!(*shaders).contains(settings::START_SYSTEM_RESOURCES))
        CreateShader(settings::START_SYSTEM_RESOURCES, "standard.vert", {.num_uniform_buffers = 1});

    if (!(*shaders).contains(settings::START_SYSTEM_RESOURCES + 1))
        CreateShader(settings::START_SYSTEM_RESOURCES + 1, "opaque.frag", {.num_samplers = 1});

    if (!(*shaders).contains(settings::START_SYSTEM_RESOURCES + 2))
        CreateShader(settings::START_SYSTEM_RESOURCES + 2, "accumulation.frag", {.num_samplers = 1});

    if (!(*shaders).contains(settings::START_SYSTEM_RESOURCES + 3))
        CreateShader(settings::START_SYSTEM_RESOURCES + 3, "resolve.frag", {.num_samplers = 1, .num_storage_textures = 2});

    if (!(*shaders).contains(settings::START_SYSTEM_RESOURCES + 4))
        CreateShader(settings::START_SYSTEM_RESOURCES + 4, "composite.vert", {});

    if (!(*shaders).contains(settings::START_SYSTEM_RESOURCES + 5))
        CreateShader(settings::START_SYSTEM_RESOURCES + 5, "composite.frag", {.num_samplers = 3});

    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};

    SDL_GPUGraphicsPipelineTargetInfo targetInfo{};
    SDL_GPUColorTargetDescription*    colorTargets = new SDL_GPUColorTargetDescription[2];
    SDL_GPUColorTargetBlendState      blendState{};

    blendState.enable_blend          = 1;
    blendState.src_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    blendState.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    blendState.color_blend_op        = SDL_GPU_BLENDOP_ADD;

    blendState.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    blendState.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    blendState.alpha_blend_op        = SDL_GPU_BLENDOP_ADD;

    colorTargets[0].blend_state = blendState;
    colorTargets[0].format      = SDL_GPU_TEXTUREFORMAT_R32G32B32A32_FLOAT;

    colorTargets[1].format      = SDL_GPU_TEXTUREFORMAT_R32_FLOAT;
    colorTargets[1].blend_state = blendState;

    targetInfo.color_target_descriptions = colorTargets;
    targetInfo.num_color_targets         = 2;
    targetInfo.has_depth_stencil_target  = 1;
    targetInfo.depth_stencil_format      = gpu->depthFormat;

    std::vector<SDL_GPUVertexAttribute>         attributes{};
    std::vector<SDL_GPUVertexBufferDescription> descriptions{};

    attributes.emplace_back(0, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Vertex, position));
    attributes.emplace_back(1, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(Vertex, uv));
    descriptions.emplace_back(0, sizeof(Vertex), SDL_GPU_VERTEXINPUTRATE_VERTEX, 0);

    attributes.emplace_back(2, 1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Instance, position));
    attributes.emplace_back(3, 1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(Instance, size));
    attributes.emplace_back(4, 1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Instance, origin));
    attributes.emplace_back(5, 1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Instance, rotationX));
    attributes.emplace_back(6, 1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Instance, rotationY));
    attributes.emplace_back(7, 1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Instance, rotationZ));
    attributes.emplace_back(8, 1, SDL_GPU_VERTEXELEMENTFORMAT_UINT, offsetof(Instance, color));
    attributes.emplace_back(9, 1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(Instance, cutPos));
    attributes.emplace_back(10, 1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(Instance, cutSize));
    descriptions.emplace_back(1, sizeof(Instance), SDL_GPU_VERTEXINPUTRATE_INSTANCE, 0);

    SDL_GPUVertexInputState inputState{};
    inputState.vertex_attributes          = attributes.data();
    inputState.num_vertex_attributes      = static_cast<Uint32>(attributes.size());
    inputState.vertex_buffer_descriptions = descriptions.data();
    inputState.num_vertex_buffers         = static_cast<Uint32>(descriptions.size());

    SDL_GPURasterizerState rsState{};
    rsState.cull_mode  = SDL_GPU_CULLMODE_NONE;
    rsState.front_face = SDL_GPU_FRONTFACE_CLOCKWISE;

    SDL_GPUMultisampleState msState{};
    msState.sample_count = gpu->sampleCount;

    SDL_GPUDepthStencilState depthState{};
    depthState.enable_depth_write = 0;
    depthState.enable_depth_test  = 1;
    depthState.compare_op         = SDL_GPU_COMPAREOP_GREATER_OR_EQUAL;

    pipelineInfo.target_info         = targetInfo;
    pipelineInfo.vertex_input_state  = inputState;
    pipelineInfo.primitive_type      = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipelineInfo.rasterizer_state    = rsState;
    pipelineInfo.multisample_state   = msState;
    pipelineInfo.depth_stencil_state = depthState;

    if (!CreatePipeline(settings::START_SYSTEM_RESOURCES + 1, pipelineInfo, settings::START_SYSTEM_RESOURCES, settings::START_SYSTEM_RESOURCES + 2))
        return 0;

    delete[] colorTargets;

    SDL_GPUColorTargetDescription resolveTarget{};
    resolveTarget.format = gpu->textureFormat;

    blendState.enable_blend = 1;

    resolveTarget.blend_state = blendState;

    targetInfo.color_target_descriptions = &resolveTarget;
    targetInfo.num_color_targets         = 1;

    pipelineInfo.target_info = targetInfo;

    if (!CreatePipeline(settings::START_SYSTEM_RESOURCES + 2, pipelineInfo, settings::START_SYSTEM_RESOURCES, settings::START_SYSTEM_RESOURCES + 3))
        return 0;

    SDL_GPUColorTargetDescription compositeTarget{};
    compositeTarget.format  = SDL_GetGPUSwapchainTextureFormat(gpu->device, window->window);
    blendState.enable_blend = 1;

    blendState.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
    blendState.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
    blendState.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

    targetInfo.has_depth_stencil_target = 0;
    depthState.enable_depth_test        = 0;
    compositeTarget.blend_state         = blendState;

    targetInfo.color_target_descriptions = &compositeTarget;
    rsState.cull_mode                    = SDL_GPU_CULLMODE_FRONT;

    pipelineInfo.target_info         = targetInfo;
    pipelineInfo.rasterizer_state    = rsState;
    pipelineInfo.depth_stencil_state = depthState;

    if (!CreatePipeline(settings::START_SYSTEM_RESOURCES + 3, pipelineInfo, settings::START_SYSTEM_RESOURCES + 4, settings::START_SYSTEM_RESOURCES + 5))
        return 0;

    depthState.enable_depth_write = 1;
    depthState.enable_depth_test  = 1;

    rsState.cull_mode = SDL_GPU_CULLMODE_BACK;

    targetInfo.has_depth_stencil_target = 1;

    pipelineInfo.target_info         = targetInfo;
    pipelineInfo.depth_stencil_state = depthState;
    pipelineInfo.rasterizer_state    = rsState;

    if (!CreatePipeline(settings::START_SYSTEM_RESOURCES, pipelineInfo, settings::START_SYSTEM_RESOURCES, settings::START_SYSTEM_RESOURCES + 1)) return 0;

    new Buffer(settings::START_SYSTEM_RESOURCES, {.usage = SDL_GPU_BUFFERUSAGE_VERTEX, .size = 4 * sizeof(Vertex)});
    new Buffer(settings::START_SYSTEM_RESOURCES + 1, {.usage = SDL_GPU_BUFFERUSAGE_INDEX, .size = 6 * sizeof(Uint16)});

    new TransferBuffer(settings::START_SYSTEM_RESOURCES, {.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = settings::DEFAULT_TEXTURETRANSFERBUFFER_SIZE});
    new TransferBuffer(settings::START_SYSTEM_RESOURCES + 1, {.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = settings::DEFAULT_STATIC_SIZE});

    // Upload Index and Vertex Data
    {
        std::vector<Vertex> vertices = {
            {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // Top-left
            {{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // Top-right
            {{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}}, // Bottom-right
            {{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}, // Bottom-left
        };
        std::vector<Uint16> indices = {0, 1, 2, 2, 3, 0};

        TransferBuffer* transferBuffer = (*transferBuffers)[settings::START_SYSTEM_RESOURCES + 1];
        Buffer*         vertexBuffer   = (*buffers)[settings::START_SYSTEM_RESOURCES];
        Buffer*         indexBuffer    = (*buffers)[settings::START_SYSTEM_RESOURCES + 1];

        auto transferBufferPtr = static_cast<Uint8*>(SDL_MapGPUTransferBuffer(gpu->device, transferBuffer->transferBuffer, 1));
        std::memcpy(transferBufferPtr, vertices.data(), vertices.size() * sizeof(Vertex));
        std::memcpy(transferBufferPtr + vertices.size() * sizeof(Vertex), indices.data(), indices.size() * sizeof(Uint16));

        SDL_UnmapGPUTransferBuffer(gpu->device, transferBuffer->transferBuffer);

        AcquireCommandBuffer();

        SDL_GPUCopyPass*              copyPass = SDL_BeginGPUCopyPass(gpu->cCmdBuffer);
        SDL_GPUTransferBufferLocation src{};
        src.transfer_buffer = transferBuffer->transferBuffer;
        src.offset          = 0;

        SDL_GPUBufferRegion dst{};
        dst.buffer = vertexBuffer->buffer;
        dst.size   = vertices.size() * sizeof(Vertex);
        dst.offset = 0;
        SDL_UploadToGPUBuffer(copyPass, &src, &dst, 1);

        src.offset = vertices.size() * sizeof(Vertex);
        dst.buffer = indexBuffer->buffer;
        dst.size   = indices.size() * sizeof(Uint16);
        SDL_UploadToGPUBuffer(copyPass, &src, &dst, 1);

        SDL_EndGPUCopyPass(copyPass);
        SubmitCommandBuffer();
    }

    CreateTextures();

    new Sampler(settings::START_SYSTEM_RESOURCES,
                {.min_filter        = SDL_GPU_FILTER_NEAREST,
                 .mag_filter        = SDL_GPU_FILTER_NEAREST,
                 .mipmap_mode       = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
                 .address_mode_u    = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
                 .address_mode_v    = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
                 .address_mode_w    = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
                 .max_anisotropy    = 16.0f,
                 .max_lod           = 1000.f,
                 .enable_anisotropy = 1});

    Log(0, "Successfully created the standard context");
    return 1;
}

void StartLoop(void (*mainFunction)(), const uint16_t frameRate) {
    uint64_t currentTime = SDL_GetTicksNS();
    uint64_t lastFrame   = currentTime;

    window->frameCount    = 0;
    window->timeMS        = 0;
    long double frameTime = (1000000000.0 / frameRate) / 1000;

    SDL_Event e;

    cContext->running = 1;
    window->deltaTime = 0;

    while (cContext->running) {
        lastFrame = currentTime;

        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_EVENT_QUIT:
                cContext->running = 0;
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                window->width  = e.window.data1;
                window->height = e.window.data2;

                CreateTextures();
                break;
            default:
                window->events.push(e);
                break;
            }
        }

        if (cContext->linkToPlugin)
            cContext->linkToPlugin();

        mainFunction();

        currentTime       = SDL_GetTicksNS();
        window->deltaTime = (currentTime - lastFrame) / 1000000.0;
        window->timeMS += window->deltaTime;

        if (frameRate && window->deltaTime < frameTime) {
            SDL_DelayPrecise(window->deltaTime);
        }
        window->frameCount++;
    }

    Log(0, "Loop finished!");
}

bool Render(const id_t cameraId) {
    if (!(*cameras).contains(cameraId)) {
        Log(1, std::format("Camera at id {} does not exist", cameraId));
        return 0;
    }
    Camera* camera = (*cameras)[cameraId];

    if (!gpu->cCmdBuffer) AcquireCommandBuffer();

    SDL_GPUTexture* swapchainTexture;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(gpu->cCmdBuffer, window->window, &swapchainTexture, &window->width, &window->height)) {
        Log(2, std::format("Could not acquire swapchain texture {}", SDL_GetError()));
        return 0;
    }

    if (swapchainTexture && (window->width && window->height)) {
        if (camera->changedProj) {
            if (camera->ddd)
                camera->proj = CreateProjectionMatrix(camera->fovY, camera->aspect, camera->near, camera->far);
            else
                camera->proj = Create2DProjectionMatrix(camera->fovY, camera->aspect);

            camera->changedProj = 0;
        }

        if (camera->changedView) {
            camera->view = CreateViewMatrix(camera->eye, camera->target, camera->up);

            camera->changedView = 0;
        }

        std::vector<SDL_GPUColorTargetInfo> colorTargets{};

        colorTargets.push_back(SDL_GPUColorTargetInfo());

        colorTargets.front().texture     = textures->at(settings::START_SYSTEM_RESOURCES)->texture;
        colorTargets.front().cycle       = 1;
        colorTargets.front().store_op    = SDL_GPU_STOREOP_STORE;
        colorTargets.front().clear_color = {0.0f, 0.0f, 0.0f, 0.0f};
        colorTargets.front().load_op     = SDL_GPU_LOADOP_CLEAR;

        SDL_GPUDepthStencilTargetInfo depthTargetInfo{};
        depthTargetInfo.texture  = (*textures)[settings::START_SYSTEM_RESOURCES + 1]->texture;
        depthTargetInfo.load_op  = SDL_GPU_LOADOP_CLEAR;
        depthTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

        SDL_PushGPUVertexUniformData(gpu->cCmdBuffer, 0, mat4f(camera->proj * camera->view).data(), sizeof(mat4f));

        SDL_GPUBufferBinding vertexBindings[2];
        vertexBindings[0].buffer = (*buffers)[settings::START_SYSTEM_RESOURCES]->buffer;
        vertexBindings[0].offset = 0;

        SDL_GPUBufferBinding indexBinding{};
        indexBinding.buffer = (*buffers)[settings::START_SYSTEM_RESOURCES + 1]->buffer;
        indexBinding.offset = 0;

        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(gpu->cCmdBuffer, colorTargets.data(), 1, &depthTargetInfo);

        SDL_BindGPUIndexBuffer(renderPass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_16BIT);
        SDL_BindGPUGraphicsPipeline(renderPass, (*pipelines)[settings::START_SYSTEM_RESOURCES]->pipeline);

        for (id_t workspaceId : camera->enabledWorkspaces) {
            Workspace* workspace = workspaces->at(workspaceId);

            if (!workspace->enabled)
                continue;

            for (TransferBuffer* transferBuffer : workspace->transferBuffers) {
                if (!transferBuffer->enabled)
                    continue;
                Buffer* instanceBuffer = (*buffers)[transferBuffer->instanceBufferId];

                vertexBindings[1].buffer = instanceBuffer->buffer;
                vertexBindings[1].offset = transferBuffer->offsetInstanceBuffer;
                SDL_BindGPUVertexBuffers(renderPass, 0, vertexBindings, 2);

                Uint32 instance_offset = 0;

                for (auto texture : transferBuffer->textures) {
                    if (texture->objects.empty()) {
                        continue;
                    }

                    SDL_GPUTextureSamplerBinding texBind{};
                    texBind.sampler = samplers->at(texture->samplerId)->sampler;
                    texBind.texture = texture->texture;

                    SDL_BindGPUFragmentSamplers(renderPass, 0, &texBind, 1);

                    auto num_instances = static_cast<Uint32>(texture->objects.size());
                    SDL_DrawGPUIndexedPrimitives(renderPass, 6, num_instances, 0, 0, instance_offset);

                    instance_offset += num_instances;
                }

                for (auto text : transferBuffer->texts) {
                    if (text->letters.empty()) {
                        continue;
                    }
                    if (!text->enabled) {
                        instance_offset += text->letters.size();
                    }

                    SDL_GPUTextureSamplerBinding texBind{};
                    texBind.sampler = samplers->at(settings::START_SYSTEM_RESOURCES)->sampler;
                    texBind.texture = text->letterAtlas;

                    SDL_BindGPUFragmentSamplers(renderPass, 0, &texBind, 1);
                    SDL_DrawGPUIndexedPrimitives(renderPass, 6, text->letters.size(), 0, 0, instance_offset);
                    instance_offset += text->letters.size();
                }
            }
        }

        SDL_EndGPURenderPass(renderPass);

        colorTargets.clear();

        colorTargets.push_back(SDL_GPUColorTargetInfo());
        colorTargets.push_back(SDL_GPUColorTargetInfo());

        colorTargets.front().texture     = textures->at(settings::START_SYSTEM_RESOURCES + 2)->texture;
        colorTargets.front().cycle       = 1;
        colorTargets.front().store_op    = SDL_GPU_STOREOP_STORE;
        colorTargets.front().clear_color = {0.0f, 0.0f, 0.0f, 0.0f};
        colorTargets.front().load_op     = SDL_GPU_LOADOP_CLEAR;

        colorTargets.back().texture     = textures->at(settings::START_SYSTEM_RESOURCES + 3)->texture;
        colorTargets.back().cycle       = 1;
        colorTargets.back().store_op    = SDL_GPU_STOREOP_STORE;
        colorTargets.back().clear_color = {0.0f, 0.0f, 0.0f, 0.0f};
        colorTargets.back().load_op     = SDL_GPU_LOADOP_CLEAR;

        depthTargetInfo.load_op = SDL_GPU_LOADOP_LOAD;

        renderPass = SDL_BeginGPURenderPass(gpu->cCmdBuffer, colorTargets.data(), 2, &depthTargetInfo);

        SDL_BindGPUIndexBuffer(renderPass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_16BIT);
        SDL_BindGPUGraphicsPipeline(renderPass, (*pipelines)[settings::START_SYSTEM_RESOURCES + 1]->pipeline);

        for (id_t workspaceId : camera->enabledWorkspaces) {
            Workspace* workspace = workspaces->at(workspaceId);

            if (!workspace->enabled)
                continue;

            for (TransferBuffer* transferBuffer : workspace->transferBuffers) {
                if (!transferBuffer->enabled)
                    continue;
                Buffer* instanceBuffer = (*buffers)[transferBuffer->instanceBufferId];

                vertexBindings[1].buffer = instanceBuffer->buffer;
                vertexBindings[1].offset = transferBuffer->offsetInstanceBuffer;
                SDL_BindGPUVertexBuffers(renderPass, 0, vertexBindings, 2);

                Uint32 instance_offset = 0;

                for (auto texture : transferBuffer->textures) {
                    if (texture->objects.empty()) {
                        continue;
                    }

                    SDL_GPUTextureSamplerBinding texBind{};
                    texBind.sampler = samplers->at(texture->samplerId)->sampler;
                    texBind.texture = texture->texture;

                    SDL_BindGPUFragmentSamplers(renderPass, 0, &texBind, 1);
                    auto num_instances = static_cast<Uint32>(texture->objects.size());
                    SDL_DrawGPUIndexedPrimitives(renderPass, 6, num_instances, 0, 0, instance_offset);

                    instance_offset += num_instances;
                }

                for (auto text : transferBuffer->texts) {
                    if (text->letters.empty()) {
                        continue;
                    }
                    if (!text->enabled) {
                        instance_offset += text->letters.size();
                    }

                    SDL_GPUTextureSamplerBinding texBind{};
                    texBind.sampler = samplers->at(settings::START_SYSTEM_RESOURCES)->sampler;
                    texBind.texture = text->letterAtlas;

                    SDL_BindGPUFragmentSamplers(renderPass, 0, &texBind, 1);
                    SDL_DrawGPUIndexedPrimitives(renderPass, 6, text->letters.size(), 0, 0, instance_offset);
                    instance_offset += text->letters.size();
                }
            }
        }

        SDL_EndGPURenderPass(renderPass);

        std::vector<SDL_GPUTexture*> storageTextures{};
        storageTextures.push_back(textures->at(settings::START_SYSTEM_RESOURCES + 2)->texture);
        storageTextures.push_back(textures->at(settings::START_SYSTEM_RESOURCES + 3)->texture);

        depthTargetInfo.load_op          = SDL_GPU_LOADOP_LOAD;
        colorTargets.front().texture     = textures->at(settings::START_SYSTEM_RESOURCES + 4)->texture;
        colorTargets.front().clear_color = {0.0f, 0.0f, 0.0f, 0.0f};
        colorTargets.front().cycle       = 1;
        colorTargets.front().store_op    = SDL_GPU_STOREOP_STORE;

        renderPass = SDL_BeginGPURenderPass(gpu->cCmdBuffer, colorTargets.data(), 1, &depthTargetInfo);
        SDL_BindGPUFragmentStorageTextures(renderPass, 0, storageTextures.data(), 2);
        SDL_BindGPUIndexBuffer(renderPass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_16BIT);
        SDL_BindGPUGraphicsPipeline(renderPass, (*pipelines)[settings::START_SYSTEM_RESOURCES + 2]->pipeline);

        for (id_t workspaceId : camera->enabledWorkspaces) {
            Workspace* workspace = workspaces->at(workspaceId);

            if (!workspace->enabled)
                continue;

            for (TransferBuffer* transferBuffer : workspace->transferBuffers) {
                if (!transferBuffer->enabled)
                    continue;
                Buffer* instanceBuffer = (*buffers)[transferBuffer->instanceBufferId];

                vertexBindings[1].buffer = instanceBuffer->buffer;
                vertexBindings[1].offset = transferBuffer->offsetInstanceBuffer;
                SDL_BindGPUVertexBuffers(renderPass, 0, vertexBindings, 2);

                Uint32 instance_offset = 0;

                for (auto texture : transferBuffer->textures) {
                    if (texture->objects.empty()) {
                        continue;
                    }

                    SDL_GPUTextureSamplerBinding texBind{};
                    texBind.sampler = samplers->at(texture->samplerId)->sampler;
                    texBind.texture = texture->texture;

                    SDL_BindGPUFragmentSamplers(renderPass, 0, &texBind, 1);

                    auto num_instances = static_cast<Uint32>(texture->objects.size());
                    SDL_DrawGPUIndexedPrimitives(renderPass, 6, num_instances, 0, 0, instance_offset);

                    instance_offset += num_instances;
                }

                for (auto text : transferBuffer->texts) {
                    if (text->letters.empty()) {
                        continue;
                    }
                    if (!text->enabled) {
                        instance_offset += text->letters.size();
                    }

                    SDL_GPUTextureSamplerBinding texBind{};
                    texBind.sampler = samplers->at(settings::START_SYSTEM_RESOURCES)->sampler;
                    texBind.texture = text->letterAtlas;

                    SDL_BindGPUFragmentSamplers(renderPass, 0, &texBind, 1);
                    SDL_DrawGPUIndexedPrimitives(renderPass, 6, text->letters.size(), 0, 0, instance_offset);
                    instance_offset += text->letters.size();
                }
            }
        }
        SDL_EndGPURenderPass(renderPass);

        colorTargets.front().texture     = swapchainTexture;
        colorTargets.front().load_op     = SDL_GPU_LOADOP_CLEAR;
        colorTargets.front().clear_color = settings::CLEAR_COLOR;
        colorTargets.front().store_op    = SDL_GPU_STOREOP_STORE;

        renderPass = SDL_BeginGPURenderPass(gpu->cCmdBuffer, colorTargets.data(), 1, &depthTargetInfo);

        SDL_BindGPUGraphicsPipeline(renderPass, pipelines->at(settings::START_SYSTEM_RESOURCES + 3)->pipeline);

        Sampler*                                  sampler = samplers->at(settings::START_SYSTEM_RESOURCES);
        std::vector<SDL_GPUTextureSamplerBinding> samplerBindings{};
        samplerBindings.push_back(
            {.texture = textures->at(settings::START_SYSTEM_RESOURCES + 4)->texture,
             .sampler = sampler->sampler});
        samplerBindings.push_back(
            {.texture = textures->at(settings::START_SYSTEM_RESOURCES + 3)->texture,
             .sampler = sampler->sampler});
        samplerBindings.push_back(
            {.texture = textures->at(settings::START_SYSTEM_RESOURCES)->texture,
             .sampler = sampler->sampler});

        SDL_BindGPUFragmentSamplers(renderPass, 0, samplerBindings.data(), 3);
        SDL_BindGPUIndexBuffer(renderPass, &indexBinding, SDL_GPU_INDEXELEMENTSIZE_16BIT);

        SDL_DrawGPUIndexedPrimitives(renderPass, 6, 1, 0, 0, 0);

        SDL_EndGPURenderPass(renderPass);
    }

    SubmitCommandBuffer();

    Log(0, "Successfully completed render pass");
    return 1;
};

} // namespace pg
