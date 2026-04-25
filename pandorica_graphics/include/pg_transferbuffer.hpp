#pragma once
#include "pg_generic.hpp"
#include "pg_text.hpp"
#include "pg_texture.hpp"

/** @file */
namespace pg {

/**
 * @brief A transferbuffer used for uploading data to the gpu.
 *
 * A transferbuffer is used to copy data from the CPU to the GPU. This has (most of the times) to be done with a CopyPass.
 *
 * A transferbuffer is part of the GPUDevice and is referenced by textures and texts.
 *
 * @see GPUDevice, Texture, Text, CreateTransferBuffer(), DeleteTransferBuffer(), ResizeTransferBuffer(), UploadData(), SetTransferBufferAttributes(), GetTransferBuffer()
 *
 * @ingroup pg_gpu
 */
struct TransferBuffer : EngineResource {
    SDL_GPUTransferBuffer* transferBuffer; /**< Pointer to the actual transferbuffer. */

    id_t                       instanceBufferId     = 0;                       /**< The id of the instancebuffer the transferbuffer is using. */
    size_t                     offsetInstanceBuffer = 0;                       /**< The offset the transferbuffer has in that instancebuffer. */
    size_t                     size;                                           /**< The size of the transferbuffer. */
    ManagementType             management = settings::DEFAULT_MANAGEMENT_TYPE; /**< How the transferbuffer should be handled (@ref ManagementType). */
    SDL_GPUTransferBufferUsage usage;                                          /**< What the transferbuffer is used for. */

    bool enabled = 1; /**< Whether or not the instances assigned to the transferbuffer should be rendered. */

    std::list<Texture*> textures; /**< List of all textures assigned to this transferbuffer. */
    std::list<Text*>    texts;    /**< List of all texts assigned to this transferbuffer. */

    size_t cOffset     = 0; /**< @internal offset of the texture data. */
    id_t   workspaceId = 0; /**< The id of the workspace the transferbuffer is assigned to. */

    TransferBuffer(id_t id, const SDL_GPUTransferBufferCreateInfo& info);
    ~TransferBuffer();
};
extern idmap<TransferBuffer>* transferBuffers; /**< Pointer to 'gpu->transferbuffers'. */

/**
 * @brief Creates a transferbuffer
 *
 * Creates a transferbuffer at the specified id.
 *
 * For most cases the usage should be upload and the size can be left empty if there is a default management mode.
 *
 * @param transferBufferId The id at which to create the transferbuffer.
 * @param info Info for creating the transferbuffer.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see TransferBuffer, DeleteTransferBuffer()
 *
 * @ingroup pg_gpu
 */
bool CreateTransferBuffer(const id_t transferBufferId, SDL_GPUTransferBufferCreateInfo info);
/**
 * @brief Deletes a transferbuffer.
 *
 * @param transferBufferId The id of the transferbuffer to delete.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see TransferBuffer, CreateTransferBuffer()
 *
 * @ingroup pg_gpu
 */
bool DeleteTransferBuffer(const id_t transferBufferId);
/**
 * @brief Resizes a transferbuffer.
 *
 * @note Transferbuffers are usually automatically resized to their needs if they have a valid management type.
 *
 * @param transferBufferId The id of the transferbuffer to resize.
 * @param size The new size for the transferbuffer.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see TransferBuffer
 *
 * @ingroup pg_gpu
 */
bool ResizeTransferBuffer(const id_t transferBufferId, const size_t size);
/**
 * @brief Uploads object and text data to the instance buffer.
 *
 * With this function you can upload the objects and texts that are assigned to the transferbuffer(s) to the gpu.
 *
 * Here the rotation and color is calculated and the letters of the text are given the real position.
 *
 * @param transferBufferIds A vector of ids for the transferbuffers which should be uploaded.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see TransferBuffer, Buffer, Object, Text
 *
 * @ingroup pg_gpu
 */
bool UploadData(const std::vector<id_t>& transferBufferIds);
/**
 * @brief Change attributes of a transferbuffer.
 *
 * Supported attribute(s): \n
 * INSTANCEBUFFER_ID, INSTANCEBUFFER_OFFSET, WORKSPACE_ID, MANAGEMENT_TYPE
 *
 * @note To resize a transferbuffer use @ref ResizeTransferBuffer()
 *
 * @param transferBufferId The id of the transferbuffer to alter.
 * @param attributes A vector of attributes to change and their new values.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see TransferBuffer, ResizeTransferBuffer()
 *
 * @ingroup pg_gpu
 */
bool SetTransferBufferAttributes(const id_t transferBufferId, const std::vector<attributeAndValue>& attributes);
/**
 * @brief Get a transferBuffer.
 *
 * @param transferBufferId The id of the transferbuffer to return.
 * @return A pointer to the transferbuffer on success or NULL on failure. See the logs for more information.
 *
 * @see TransferBuffer
 *
 * @ingroup pg_gpu
 */
TransferBuffer* GetTransferBuffer(const id_t transferBufferId);

} // namespace pg
