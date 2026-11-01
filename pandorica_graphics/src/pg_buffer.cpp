#include "../include/pg_buffer.hpp"
#include "../include/pg.hpp"

namespace pg {

idmap<Buffer>* buffers;

Buffer::Buffer(id_t id, const SDL_GPUBufferCreateInfo& info) : EngineResource(id) {
    this->buffer   = SDL_CreateGPUBuffer(gpu->device, &info);
    this->size     = info.size;
    this->usage    = info.usage;
    (*buffers)[id] = this;
}
Buffer::~Buffer() {
    SDL_ReleaseGPUBuffer(gpu->device, this->buffer);
    (*buffers).erase(id);
}

bool CreateBuffer(const id_t bufferId, SDL_GPUBufferCreateInfo info) {
    if (!bufferId) {
        Log(1, "BufferId is not allowed to be 0");
        return 0;
    }

    if (buffers->contains(bufferId)) {
        Log(1, std::format("Buffer at id {} already exists, overwriting it", bufferId));
        delete (*buffers)[bufferId];
    }

    if (!info.usage)
        info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;

    if (!info.size)
        info.size = (settings::DEFAULT_MANAGEMENT_TYPE == MANAGEMENT_STATIC ? settings::DEFAULT_STATIC_SIZE : settings::DEFAULT_DYNAMIC_SIZE);

    if (!(new Buffer(bufferId, info))->buffer) {
        Log(1, SDL_GetError());
        return 0;
    }

    Log(0, std::format("Successfully created buffer at id {}", bufferId));
    return 1;
}

bool DeleteBuffer(const id_t bufferId) {
    if (!buffers->contains(bufferId)) {
        Log(1, std::format("Buffer at id {} does not exist, exiting delete function", bufferId));
        return 0;
    }

    delete (*buffers)[bufferId];

    Log(0, std::format("Successfully deleted buffer at id {}", bufferId));
    return 1;
}

bool SetBufferAttributes(const id_t bufferId, const std::vector<attributeAndValue>& attributes) {
    if (!buffers->contains(bufferId)) {
        Log(1, std::format("Buffer at id {} does not exist, cannot set its attributes", bufferId));
        return 0;
    }
    Buffer* buffer = buffers->at(bufferId);
    bool    error  = 1;

    {
        for (auto i : attributes) {
            switch (i.first) {
            case MANAGEMENT_TYPE:
                buffer->management = static_cast<ManagementType>(i.second);
                break;
            default:
                Log(1, "This attribute is not supported for buffers");
                error = 0;
                break;
            }
        }
    }

    Log(0, std::format("Successfully set attributes for buffer at id {}", bufferId));
    return error;
}

bool ResizeBuffer(const id_t bufferId, const size_t size) {
    if (!buffers->contains(bufferId)) {
        Log(1, std::format("Buffer at id {} does not exist, cannot resize buffer", bufferId));
        return 0;
    }

    if (!size) {
        Log(1, "Size is 0, deleting buffer");
        delete (*buffers)[bufferId];
        return 1;
    }

    if (!gpu->cCmdBuffer) AcquireCommandBuffer();

    Buffer* buffer = (*buffers)[bufferId];

    {
        SDL_GPUBufferCreateInfo info;
        info.size                = size;
        info.usage               = buffer->usage;
        SDL_GPUBuffer* SDLBuffer = SDL_CreateGPUBuffer(gpu->device, &info);

        SDL_GPUBufferLocation src{};
        src.buffer = buffer->buffer;
        src.offset = 0;

        SDL_GPUBufferLocation dst{};
        dst.buffer = SDLBuffer;
        dst.offset = 0;

        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(gpu->cCmdBuffer);
        SDL_CopyGPUBufferToBuffer(copyPass, &src, &dst, std::min(size, buffer->size), 1);
        SDL_EndGPUCopyPass(copyPass);

        SDL_ReleaseGPUBuffer(gpu->device, buffer->buffer);
        buffer->buffer = SDLBuffer;
        buffer->size   = size;
    }

    Log(0, std::format("Successfully resized buffer at id {}", bufferId));
    return 1;
}

Buffer* GetBuffer(const id_t bufferId) {
    if (!buffers->contains(bufferId)) {
        Log(1, std::format("Buffer at id {} does not exist, cannot get it", bufferId));
        return nullptr;
    }

    return (*buffers)[bufferId];
}

} // namespace pg
