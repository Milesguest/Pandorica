#pragma once
#include "pg_generic.hpp"

/** @file */
namespace pg {
/**
 * @brief A struct for memory space on the VRAM.
 *
 * The buffer is memory space on the VRAM.
 * By default all buffers should be created with the vertex usage flag,
 * only use a different one if you know what you're doing.
 *
 * If you want to upload data to the buffer, you have to use a @ref TransferBuffer.
 *
 * Buffers are part of the GPUDevice and can be referenced by transferbuffers.
 *
 * @note A vertex and index buffer with correct data is automatically created in @ref CreateStandardContext().
 *
 * @see GPUDevice, TransferBuffer, CreateBuffer(), DeleteBuffer(), SetBufferAttributes(), ResizeBuffer(), GetBuffer()
 *
 * @ingroup pg_gpu
 */
struct Buffer : EngineResource {
    SDL_GPUBuffer* buffer; /**< The pointer to the actual buffer. */

    size_t                  size;                                           /**< The size of the buffer. */
    ManagementType          management = settings::DEFAULT_MANAGEMENT_TYPE; /**< The management type of the buffer. */
    SDL_GPUBufferUsageFlags usage;                                          /**< The usage of the buffer. */

    bool deleteWithTransferBuffer = 0; /**< @internal Whether or not this buffer was created for a transferBuffer. */

    Buffer(id_t id, const SDL_GPUBufferCreateInfo& info);
    ~Buffer();
};
extern idmap<Buffer>* buffers; /**< Pointer to 'gpu.buffers' */

/**
 * @brief Creates a buffer.
 *
 * @note A gpu device must have been created.
 *
 * @param bufferId The id of the newly created buffer.
 * @param info The create info of the buffer, can be left empty to use default values.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Buffer, DeleteBuffer()
 *
 * @ingroup pg_gpu
 */
bool CreateBuffer(const id_t bufferId, SDL_GPUBufferCreateInfo info);
/**
 * @brief Deletes a buffer
 *
 * @param bufferId The id of the buffer to delete.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Buffer, CreateBuffer()
 *
 * @ingroup pg_gpu
 */
bool DeleteBuffer(const id_t bufferId);
/**
 * @brief Set attributes of a buffer.
 *
 * Supported attribute(s): \n
 * MANAGEMENT_TYPE
 *
 * @note To change the size of a buffer (usually this is automatically done) use @ref ResizeBuffer().
 *
 * @param bufferId the id of the buffer to modify.
 * @param attributes A list of attributes and values to change.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Buffer, ResizeBuffer()
 *
 * @ingroup pg_gpu
 */
bool SetBufferAttributes(const id_t bufferId, const std::vector<attributeAndValue>& attributes);
/**
 * @brief Resizes a buffer.
 *
 * If there is a valid management type, buffers are automatically resized when uploading
 * data to it.
 *
 * @param bufferId The id of the buffer to resize.
 * @param size The new size of the buffer
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Buffer
 *
 * @ingroup pg_gpu
 */
bool ResizeBuffer(const id_t bufferId, const size_t size);
/**
 * @brief Get the buffer of the specified id.
 *
 * @param bufferId The id of the buffer to return.
 * @return The buffer of the specified id or NULL if it could not be found.
 *
 * @see Buffer
 *
 * @ingroup pg_gpu
 */
Buffer* GetBuffer(const id_t bufferId);

} // namespace pg
