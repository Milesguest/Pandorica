#include "../include/pg_transferbuffer.hpp"
#include "../include/pg.hpp"

namespace pg {

idmap<TransferBuffer>* transferBuffers;

TransferBuffer::TransferBuffer(id_t id, const SDL_GPUTransferBufferCreateInfo& info) : EngineResource(id) {
    this->transferBuffer   = SDL_CreateGPUTransferBuffer(cContext->gpu->device, &info);
    this->size             = info.size;
    this->usage            = info.usage;
    (*transferBuffers)[id] = this;
}
TransferBuffer::~TransferBuffer() {
    SDL_ReleaseGPUTransferBuffer(cContext->gpu->device, this->transferBuffer);
    (*transferBuffers).erase(id);
}

bool CreateTransferBuffer(const id_t transferBufferId, SDL_GPUTransferBufferCreateInfo info) {
    if (!transferBufferId) {
        Log(1, "TransferBufferId is not allowed to be 0");
        return 0;
    }

    if (cContext->gpu->transferBuffers.contains(transferBufferId)) {
        Log(1, std::format("TransferBuffer at id {} already exists, deleting it", transferBufferId));
        delete cContext->gpu->transferBuffers[transferBufferId];
    }

    if (!info.size)
        info.size = (settings::DEFAULT_MANAGEMENT_TYPE == MANAGEMENT_STATIC ? settings::DEFAULT_STATIC_SIZE : settings::DEFAULT_DYNAMIC_SIZE);

    if (!(new TransferBuffer(transferBufferId, info))->transferBuffer) {
        Log(1, SDL_GetError());
        return 0;
    }

    Log(0, std::format("Successfully created transferBuffer at id {}", transferBufferId));
    return 1;
}

bool DeleteTransferBuffer(const id_t transferBufferId) {
    if (!cContext->gpu->transferBuffers.contains(transferBufferId)) {
        Log(1, std::format("TransferBuffer at id {} does not exist, exiting delete function", transferBufferId));
        return 0;
    }
    TransferBuffer* transferBuffer = cContext->gpu->transferBuffers[transferBufferId];

    if (buffers->contains(transferBuffer->instanceBufferId))
        if (buffers->at(transferBuffer->instanceBufferId)->deleteWithTransferBuffer)
            DeleteBuffer(transferBuffer->instanceBufferId);

    for (Texture* texture : transferBuffer->textures)
        DeleteTexture(texture->id);

    for (Text* text : transferBuffer->texts)
        DeleteText(text->id);

    delete transferBuffer;

    Log(0, std::format("Successfully deleted transferBuffer at id {}", transferBufferId));
    return 1;
}

bool ResizeTransferBuffer(const id_t transferBufferId, const size_t size) {
    if (!cContext->gpu->transferBuffers.contains(transferBufferId)) {
        Log(1, std::format("TransferBuffer at id {} does not exist, cannot resize transferBuffer", transferBufferId));
        return 0;
    }

    if (!size) {
        Log(1, "Size = 0, deleting transferBuffer");
        delete cContext->gpu->transferBuffers[transferBufferId];
        return 1;
    }

    TransferBuffer* transferBuffer = transferBuffers->at(transferBufferId);

    {
        SDL_GPUTransferBufferCreateInfo info{};
        info.size  = size;
        info.usage = transferBuffer->usage;

        SDL_GPUTransferBuffer* SDLTransferBuffer = SDL_CreateGPUTransferBuffer(gpu->device, &info);

        if (transferBuffer->cOffset) {
            auto oldTbPointer = SDL_MapGPUTransferBuffer(gpu->device, transferBuffer->transferBuffer, 1);
            auto newTbPointer = SDL_MapGPUTransferBuffer(gpu->device, SDLTransferBuffer, 1);

            std::memcpy(newTbPointer, oldTbPointer, transferBuffer->cOffset);

            SDL_UnmapGPUTransferBuffer(gpu->device, transferBuffer->transferBuffer);
            SDL_UnmapGPUTransferBuffer(gpu->device, SDLTransferBuffer);
        }

        SDL_ReleaseGPUTransferBuffer(gpu->device, transferBuffer->transferBuffer);
        transferBuffer->transferBuffer = SDLTransferBuffer;
        transferBuffer->size           = size;
    }

    Log(0, std::format("Successfully resized transferBuffer at id {}", transferBufferId));
    return 1;
}

bool UploadData(const std::vector<id_t>& transferBufferIds) {
    if (transferBufferIds.empty()) {
        Log(1, "TransferBufferIds is empty, exiting upload data function");
        return 0;
    }

    if (!gpu->cCmdBuffer) AcquireCommandBuffer();

    for (id_t transferBufferId : transferBufferIds) {
        if (!transferBuffers->contains(transferBufferId)) {
            Log(1, std::format("TransferBuffer at id {} does not exist, cannot upload its data", transferBufferId));
            return 0;
        }
        TransferBuffer* transferBuffer = transferBuffers->at(transferBufferId);

        if (!transferBuffer->instanceBufferId) {
            Log(0, "InstanceBufferId is 0, creating a new buffer");
            transferBuffer->instanceBufferId = GetLowestKey(buffers, settings::START_SYSTEM_RESOURCES);
            CreateBuffer(transferBuffer->instanceBufferId, {.usage = SDL_GPU_BUFFERUSAGE_VERTEX, .size = static_cast<Uint32>(transferBuffer->size)});
            buffers->at(transferBuffer->instanceBufferId)->management               = transferBuffer->management;
            buffers->at(transferBuffer->instanceBufferId)->deleteWithTransferBuffer = 1;
        }

        if (!buffers->contains(transferBuffer->instanceBufferId)) {
            Log(1, std::format("Buffer at id {} does not exist, cannot upload data to it", transferBuffer->instanceBufferId));
            return 0;
        }
        Buffer* instanceBuffer = buffers->at(transferBuffer->instanceBufferId);

        // Resize transfer-/instanceBuffer if necessary
        size_t dataSize = 0;
        for (Texture* texture : transferBuffer->textures)
            dataSize += texture->objects.size();

        for (Text* text : transferBuffer->texts)
            dataSize += text->letters.size();

        dataSize *= sizeof(Instance);

        if (transferBuffer->management == MANAGEMENT_DYNAMIC && dataSize + settings::DEFAULT_DYNAMIC_SIZE < transferBuffer->size) {
            size_t newSize = (dataSize / settings::DEFAULT_DYNAMIC_SIZE + 1) * settings::DEFAULT_DYNAMIC_SIZE;
            ResizeTransferBuffer(transferBufferId, newSize);
        } else if (dataSize > transferBuffer->size) {
            if (transferBuffer->management != MANAGEMENT_UNDEFINED) {
                size_t defaultSize = (transferBuffer->management == MANAGEMENT_STATIC ? settings::DEFAULT_STATIC_SIZE : settings::DEFAULT_DYNAMIC_SIZE);
                size_t newSize     = (dataSize / defaultSize + 1) * defaultSize;
                ResizeTransferBuffer(transferBufferId, newSize);
            } else {
                Log(1, std::format("TransferBuffer at id {} is of undefined management type, cannot resize it", transferBufferId));
                return 0;
            }
        }

        if (instanceBuffer->management == MANAGEMENT_DYNAMIC && dataSize + settings::DEFAULT_DYNAMIC_SIZE < instanceBuffer->size) {
            size_t newSize = (dataSize / settings::DEFAULT_DYNAMIC_SIZE + 1) * settings::DEFAULT_DYNAMIC_SIZE;
            ResizeBuffer(instanceBuffer->id, newSize);
        } else if (instanceBuffer->size < transferBuffer->size) {
            if (instanceBuffer->management != MANAGEMENT_UNDEFINED) {
                size_t defaultSize = (instanceBuffer->management == MANAGEMENT_STATIC ? settings::DEFAULT_STATIC_SIZE : settings::DEFAULT_DYNAMIC_SIZE);
                size_t newSize     = (dataSize / defaultSize + 1) * defaultSize;
                ResizeBuffer(instanceBuffer->id, newSize);
            } else {
                Log(1, std::format("Buffer at id {} is of undefined management type, cannot resize it", instanceBuffer->id));
                return 0;
            }
        }
    }

    // Upload instance data
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(gpu->cCmdBuffer);

    for (id_t transferBufferId : transferBufferIds) {
        TransferBuffer* transferBuffer = transferBuffers->at(transferBufferId);
        Buffer*         instanceBuffer = buffers->at(transferBuffer->instanceBufferId);

        std::vector<Instance> instances{};

        for (Texture* texture : transferBuffer->textures) {
            for (Object* object : texture->objects) {

                if (object->rotated) {
                    anglef roll(object->rotation.x(), Eigen::Vector3f::UnitX());
                    anglef pitch(object->rotation.y(), Eigen::Vector3f::UnitY());
                    anglef yaw(object->rotation.z(), Eigen::Vector3f::UnitZ());

                    mat3f rotationMatrix        = (yaw * pitch * roll).toRotationMatrix();
                    object->instance->rotationX = rotationMatrix.col(0);
                    object->instance->rotationY = rotationMatrix.col(1);
                    object->instance->rotationZ = rotationMatrix.col(2);
                    object->rotated             = 0;
                }

                if (object->colorChanged) {
                    object->instance->color = static_cast<uint32_t>(object->color[3] << 24) | (object->color[2] << 16) | (object->color[1] << 8) | (object->color[0]);

                    object->colorChanged = 0;
                }

                if (!object->enabled)
                    continue;

                instances.push_back(*object->instance);
            }
        }

        for (Text* text : transferBuffer->texts) {
            if (text->instance.use_count() >= 2) {
                Object* object = objects->at(text->attachedObjectId);

                if (object->rotated) {
                    anglef roll(object->rotation.x(), Eigen::Vector3f::UnitX());
                    anglef pitch(object->rotation.y(), Eigen::Vector3f::UnitY());
                    anglef yaw(object->rotation.z(), Eigen::Vector3f::UnitZ());

                    mat3f rotationMatrix        = (yaw * pitch * roll).toRotationMatrix();
                    object->instance->rotationX = rotationMatrix.col(0);
                    object->instance->rotationY = rotationMatrix.col(1);
                    object->instance->rotationZ = rotationMatrix.col(2);
                    object->rotated             = 0;
                }

                if (object->colorChanged) {
                    object->instance->color = static_cast<uint32_t>(object->color[3] << 24) | (object->color[2] << 16) | (object->color[1] << 8) | (object->color[0]);

                    object->colorChanged = 0;
                }
            }

            for (Instance& letter : text->letters) {
                letter.position  = text->instance->position;
                letter.color     = text->instance->color;
                letter.rotationX = text->instance->rotationX;
                letter.rotationY = text->instance->rotationY;
                letter.rotationZ = text->instance->rotationZ;
            }

            if (!text->enabled)
                continue;

            instances.insert(instances.end(), text->letters.begin(), text->letters.end());
        }

        auto transferBufferPtr = SDL_MapGPUTransferBuffer(gpu->device, transferBuffer->transferBuffer, 1);

        std::memcpy(transferBufferPtr, instances.data(), instances.size() * sizeof(Instance));

        SDL_UnmapGPUTransferBuffer(gpu->device, transferBuffer->transferBuffer);

        SDL_GPUTransferBufferLocation src{};
        src.transfer_buffer = transferBuffer->transferBuffer;
        src.offset          = 0;

        SDL_GPUBufferRegion dst{};
        dst.buffer = instanceBuffer->buffer;
        dst.size   = instances.size() * sizeof(Instance);
        dst.offset = transferBuffer->offsetInstanceBuffer;

        SDL_UploadToGPUBuffer(copyPass, &src, &dst, 1);
    }

    SDL_EndGPUCopyPass(copyPass);

    Log(0, "Successfully uploaded data");
    return 1;
}

bool SetTransferBufferAttributes(const id_t transferBufferId, const std::vector<attributeAndValue>& attributes) {
    if (!cContext->gpu->transferBuffers.contains(transferBufferId)) {
        Log(1, std::format("TransferBuffer at id {} does not exist, cannot set attributes", transferBufferId));
        return 0;
    }

    TransferBuffer* transferBuffer = transferBuffers->at(transferBufferId);
    bool            error          = 1;

    for (auto i : attributes) {
        switch (i.first) {
        case INSTANCEBUFFER_ID:
            if (!buffers->contains(i.second.asUint())) {
                Log(1, std::format("Buffer at id {} does not exist, cannot set it as attribute", static_cast<id_t>(i.second)));
                error = 0;
                break;
            }
            transferBuffer->instanceBufferId = i.second.asUint();
            break;
        case INSTANCEBUFFER_OFFSET:
            transferBuffer->offsetInstanceBuffer = i.second.asUint();
            break;
        case WORKSPACE_ID:
            if (!(*workspaces).contains(i.second.asUint())) {
                Log(1, std::format("Workspace at id {} does not exist, cannot set it as attribute", static_cast<id_t>(i.second)));
                error = 0;
                break;
            }
            transferBuffer->workspaceId = i.second.asUint();

            for (Texture* texture : transferBuffer->textures) {
                texture->workspaceId = i.second.asUint();
                for (Object* object : texture->objects) {
                    object->workspaceId = i.second.asUint();
                }
            }

            (*workspaces)[i.second.asUint()]->transferBuffers.push_back(transferBuffer);

            break;
        case MANAGEMENT_TYPE:
            transferBuffer->management = static_cast<ManagementType>(i.second);
            break;
        default:
            error = 0;
            break;
        }
    }

    Log(0, std::format("Successfully set attributes for transferBuffer at id {}", transferBufferId));
    return error;
}

TransferBuffer* GetTransferBuffer(const id_t transferBufferId) {
    if (!transferBuffers->contains(transferBufferId)) {
        Log(1, std::format("TransferBuffer at id {} does not exist, cannot get it", transferBufferId));
        return nullptr;
    }

    return transferBuffers->at(transferBufferId);
}

} // namespace pg
