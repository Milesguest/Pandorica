#include "../include/pg_texture.hpp"
#include "../include/pg.hpp"

namespace pg {

idmap<Texture>* textures;

Texture::Texture(id_t id, const SDL_GPUTextureCreateInfo& info) : EngineResource(id) {
    this->texture   = SDL_CreateGPUTexture(gpu->device, &info);
    this->mipmap    = info.num_levels;
    this->size      = SDL_CalculateGPUTextureFormatSize(info.format, info.width, info.height, 1);
    this->width     = info.width;
    this->height    = info.height;
    (*textures)[id] = this;
}
Texture::~Texture() {
    SDL_ReleaseGPUTexture(gpu->device, this->texture);
    textures->erase(id);

    if (this->transferBufferId)
        transferBuffers->at(transferBufferId)->textures.remove(this);
}

void CopyTextureToTransferBuffer(Texture* texture, const SDL_Surface* surface, TransferBuffer* transferBuffer) {
    Uint8* transferBufferPtr = static_cast<Uint8*>(SDL_MapGPUTransferBuffer(gpu->device, transferBuffer->transferBuffer, 1));

    std::memcpy(transferBufferPtr + transferBuffer->cOffset, surface->pixels, texture->size);

    SDL_UnmapGPUTransferBuffer(gpu->device, transferBuffer->transferBuffer);

    texture->transferBufferId = transferBuffer->id;
    transferBuffer->cOffset += texture->size;
    transferBuffer->textures.push_front(texture);
}

bool CreateTexture(const id_t textureId, const id_t transferBufferId, const SDL_Surface* surface, SDL_GPUTextureCreateInfo info) {
    if (!textureId) {
        Log(1, "TextureId is not allowed to be 0");
        return 0;
    }

    if (!transferBuffers->contains(transferBufferId)) {
        Log(1, std::format("TransferBuffer at id {} does not exist, cannot create texture", transferBufferId));
        return 0;
    }

    if (textures->contains(textureId)) {
        Log(1, std::format("Texture at id {} already exists, deleting it", textureId));
        delete (*textures)[textureId];
    }

    TransferBuffer* transferBuffer = (*transferBuffers)[transferBufferId];

    if (!surface) {
        Log(1, "Surface is empty, cannot create texture from it");
        return 0;
    }

    info.width  = surface->w;
    info.height = surface->h;
    if (!info.format)
        info.format = gpu->textureFormat;

    if (!info.type)
        info.type = SDL_GPU_TEXTURETYPE_2D;

    if (!info.layer_count_or_depth) {
        info.layer_count_or_depth = 1;
    }

    if (!info.num_levels) {
        info.num_levels = 1;
    }

    if (!info.usage) {
        info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET;
    }

    Texture* texture = new Texture(textureId, info);
    texture->size    = SDL_CalculateGPUTextureFormatSize(gpu->textureFormat, info.width, info.height, info.layer_count_or_depth);

    if (!texture->texture) {
        Log(1, SDL_GetError());
        return 0;
    }

    CopyTextureToTransferBuffer(texture, surface, transferBuffer);

    Log(0, std::format("Successfully created texture at id {}", textureId));
    return 1;
}

bool CreateTexture(const id_t textureId, const id_t transferBufferId, const std::string& path, SDL_GPUTextureCreateInfo info) {
    Texture* texture;

    if (!textureId) {
        Log(1, "TextureId is not allowed to be 0");
        return 0;
    }

    if (textures->contains(textureId)) {
        Log(0, std::format("Texture at id {} already exists, deleting it", textureId));
        DeleteTexture(textureId);
    }

    if (path.empty()) {

        Log(0, std::format("Creating texture without image data at id {}", textureId));

        if (transferBufferId) {
            if (!transferBuffers->contains(transferBufferId)) {
                Log(1, std::format("TransferBuffer at id {} does not exist, cannot create the texture", transferBufferId));
                return 0;
            }
            TransferBuffer* transferBuffer = transferBuffers->at(transferBufferId);
            transferBuffer->textures.push_front(texture);
        }
        if (info.width && info.height) {
            texture = new Texture(textureId, info);
            if (!texture->texture) {
                Log(1, std::format("Could not create texture at id {}: {}", textureId, SDL_GetError()));
                return 0;
            }
        } else
            texture = new Texture(textureId);

        texture->width            = info.width;
        texture->height           = info.height;
        texture->transferBufferId = transferBufferId;
    } else {
        Log(0, std::format("Creating texture from {} at id {}", path, textureId));

        SDL_Surface* surface = IMG_Load(std::format("{}{}", cContext->basePath, path).c_str());
        if (!surface) {
            Log(1, std::format("Could not load image at {}", path));
            return 0;
        }

        bool error = CreateTexture(textureId, transferBufferId, surface, info);
        if (!error) {
            SDL_DestroySurface(surface);
            return error;
        }
        texture = textures->at(textureId);
    }

    texture->path = path;

    return 1;
}

bool CreateTexture(const id_t textureId, const id_t transferBufferId, const std::string& path) {
    return CreateTexture(textureId, transferBufferId, path, {.format = gpu->textureFormat, .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER | SDL_GPU_TEXTUREUSAGE_COLOR_TARGET});
}

bool DeleteTexture(const id_t textureId) {
    if (!textures->contains(textureId)) {
        Log(1, std::format("Texture at id {} does not exist, exiting delete function", textureId));
        return 0;
    }

    while (!(*textures)[textureId]->objects.empty()) {
        DeleteObject((*textures)[textureId]->objects.front()->id);
    }

    delete (*textures)[textureId];

    Log(0, std::format("Successfully deleted texture at id {}", textureId));
    return 1;
}

bool UploadTextures(const id_t textureTransferBufferId, const id_t objectTransferBufferId) {
    if (!transferBuffers->contains(textureTransferBufferId)) {
        Log(1, std::format("TransferBuffer at id {} does not exist, cannot upload textures", textureTransferBufferId));
        return 0;
    }
    if (!transferBuffers->contains(objectTransferBufferId)) {
        Log(1, std::format("TransferBuffer at id {} does not exist, cannot upload textures", objectTransferBufferId));
        return 0;
    }
    TransferBuffer* textureTransferBuffer = (*transferBuffers)[textureTransferBufferId];

    if (textureTransferBuffer->textures.empty()) {
        Log(1, std::format("No textures in transferBuffer at {}, exiting upload textures", textureTransferBufferId));
        return 0;
    }

    TransferBuffer* objectTransferBuffer = (*transferBuffers)[objectTransferBufferId];

    if (!gpu->cCmdBuffer) AcquireCommandBuffer();

    if (textureTransferBuffer->cOffset > textureTransferBuffer->size) {
        if (textureTransferBuffer->management) {
            Log(0, std::format("Resizing transferBuffer at id {}", textureTransferBufferId));

            size_t newTransferBufferSize = (static_cast<int>(textureTransferBuffer->cOffset / settings::DEFAULT_TEXTURETRANSFERBUFFER_SIZE) + 1) * settings::DEFAULT_TEXTURETRANSFERBUFFER_SIZE;

            ResizeTransferBuffer(textureTransferBufferId, newTransferBufferSize);
        } else {
            Log(1, "Not allowed to resize transferBuffer of undefined management type");
            return 0;
        }
    }

    {
        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(gpu->cCmdBuffer);

        SDL_GPUTextureTransferInfo texSrc{};
        texSrc.transfer_buffer = textureTransferBuffer->transferBuffer;
        SDL_GPUTextureRegion texDst{};
        for (Texture* texture : textureTransferBuffer->textures) {
            if (texture->path.empty())
                continue;

            texSrc.offset = (textureTransferBuffer->cOffset -= texture->size);

            texDst.texture = texture->texture;
            texDst.w       = texture->width;
            texDst.h       = texture->height;
            texDst.d       = 1;

            SDL_UploadToGPUTexture(copyPass, &texSrc, &texDst, 1);
        }

        SDL_EndGPUCopyPass(copyPass);

        for (Texture* texture : textureTransferBuffer->textures) {
            if (texture->mipmap > 1)
                SDL_GenerateMipmapsForGPUTexture(gpu->cCmdBuffer, texture->texture);

            objectTransferBuffer->textures.push_front(texture);
            texture->transferBufferId = objectTransferBuffer->id;
            texture->workspaceId      = objectTransferBuffer->workspaceId;
        }

        textureTransferBuffer->textures.clear();
        textureTransferBuffer->cOffset = 0;
    }

    Log(0, std::format("Successfully uploaded textures of transferBuffer at id {}", textureTransferBufferId));
    return 1;
}

bool SetTextureAttributes(const id_t textureId, const std::vector<attributeAndValue>& attributes) {
    if (!(*textures).contains(textureId)) {
        Log(1, std::format("Texture at id {} does not exist, can not set attributes", textureId));
        return 0;
    }

    Texture* texture = (*textures)[textureId];
    bool     error   = 1;

    for (auto i : attributes) {
        switch (i.first) {
        case TRANSFERBUFFER_ID:
            if (!(*transferBuffers).contains(i.second.asUint())) {
                Log(1, std::format("TransferBuffer at id {} does not exist, cannot set it as attribute", static_cast<id_t>(i.second)));
                error = 0;
                break;
            }
            (*transferBuffers)[texture->transferBufferId]->textures.remove(texture);
            texture->transferBufferId = i.second.asUint();
            (*transferBuffers)[i.second.asUint()]->textures.push_back(texture);
            break;
        case SAMPLER_ID:
            if (!(*samplers).contains(i.second.asUint())) {
                Log(1, std::format("Sampler at id {} does not exist, cannot set is as attribute", static_cast<id_t>(i.second)));
                error = 0;
                break;
            }
            texture->samplerId = i.second.asUint();
            break;
        default:
            Log(1, "This attribute is not supported for textures");
            error = 0;
            break;
        }
    }

    Log(0, std::format("Successfully set attributes for texture at id {}", textureId));
    return error;
}

Texture* GetTexture(const id_t textureId) {
    if (!textures->contains(textureId)) {
        Log(1, std::format("Texture at id {} does not exist, cannot get it", textureId));
        return nullptr;
    }

    return textures->at(textureId);
}

} // namespace pg
