#pragma once
#include "pg_generic.hpp"
#include "pg_object.hpp"

/** @file */
namespace pg {

struct Object;
/**
 * @brief A texture holding image data on the GPU.
 *
 * A texture is used to store image data on the gpu.
 * It is basically a buffer whose data is interpreted as an image.
 *
 * To actually use the texture you first have to call @ref CreateTexture().
 * You can do that with multiple textures and in the end you call @ref UploadTextures().
 *
 * A texture is part of the GPUDevice and is referenced by transferbuffers and objects.
 *
 * @see GPUDevice, TransferBuffer, Object, CreateTexture(), DeleteTexture()
 *
 * @ingroup pg_gpu
 */
struct Texture : EngineResource {
    SDL_GPUTexture* texture = nullptr; /**< Pointer to the texture on the GPU. */

    std::list<Object*> objects{}; /**< Vector of all objects using this texture. */

    id_t samplerId        = settings::START_SYSTEM_RESOURCES; /**< The sampler the texture should use (in the standard context, this is not used). */
    id_t transferBufferId = 0;                                /**< The id of the transferbuffer the texture is part of. */

    size_t      width;  /**< The width of the texture. */
    size_t      height; /**< The height of the texture. */
    size_t      size;   /**< The size of the texture. */
    int         mipmap; /**< The mipmap level of the texture. */
    std::string path;   /**< The relative path to the file for the texture. */

    id_t workspaceId = 0; /**< The id of the workspace the texture is part of. */

    Texture(id_t id, const SDL_GPUTextureCreateInfo& info);
    Texture(id_t id) : EngineResource(id) {};
    ~Texture();
};
extern idmap<Texture>* textures; /**< Pointer to 'gpu->textures'. */

/**
 * @brief Creates a texture.
 *
 * This function creates a texture at the specified id with the specified data (surface or path). That data is then copied to the transferbuffer.
 *
 * @param textureId The id at which to create the texture.
 * @param transferBufferId The id of the transferbuffer that is used for uploading the texture.
 * @param surface The image data of the texture (usually acquired with 'IMG_Load()').
 * @param info Info about the texture. Don't set width and height and if you want to, you can change the other attributes, although you can just leave this empty.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Texture, DeleteTexture()
 *
 * @ingroup pg_gpu
 */
bool CreateTexture(const id_t textureId, const id_t transferBufferId, const SDL_Surface* surface, SDL_GPUTextureCreateInfo info);
/**
 * @overload bool CreateTexture(const id_t textureId, const id_t transferBufferId, const SDL_Surface* surface, SDL_GPUTextureCreateInfo info)
 *
 * @param textureId The id at which to create the texture.
 * @param transferBufferId The id of the transferbuffer that is used for uploading the texture.
 * @param path The relative path at which the texture file is located.
 * @param info Info about the texture. Don't set width and height and if you want to, you can change the other attributes, although you can just leave this empty.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Texture, TransferBuffer, DeleteTexture()
 *
 * @ingroup pg_gpu
 */
bool CreateTexture(const id_t textureId, const id_t transferBufferId, const std::string& path, SDL_GPUTextureCreateInfo info);
/**
 * @overload bool CreateTexture(const id_t textureId, const id_t transferBufferId, const SDL_Surface* surface, SDL_GPUTextureCreateInfo info)
 *
 * @param textureId The id at which to create the texture.
 * @param transferBufferId The id of the transferbuffer that is used for uploading the texture.
 * @param path The relative path at which the texture file is located.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Texture, TransferBuffer, DeleteTexture()
 *
 * @ingroup pg_gpu
 */
bool CreateTexture(const id_t textureId, const id_t transferBufferId, const std::string& path);
/**
 * @brief Deletes a texture
 *
 * @note Also deletes every object with this texture.
 *
 * @param textureId The id of the texture to delete.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Texture, TransferBuffer, CreateTexture()
 *
 * @ingroup pg_gpu
 */
bool DeleteTexture(const id_t textureId);
/**
 * @brief Uploads textures to the gpu.
 *
 * You can use this to upload the data that was copied to the texture transferbuffer to the specific texture on the gpu.
 * After that the textures are assigned to the specified object transferbuffer.
 *
 * @param textureTransferBufferId The id of the transferbuffer the textures were created with.
 * @param objectTransferBufferId The id of the transferbuffer the textures should be assigned to afterwards.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Texture, TransferBuffer, CreateTexture()
 *
 * @ingroup pg_gpu
 */
bool UploadTextures(const id_t textureTransferBufferId, const id_t objectTransferBufferId);
/**
 * @brief Set attributes for a texture.
 *
 * Supported attribute(s): \n
 * TRANSFERBUFFER_ID, SAMPLER_ID
 *
 * @param textureId The id of the texture to modify.
 * @param attributes A vector of attributes to change and values.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Texture
 *
 * @ingroup pg_gpu
 */
bool SetTextureAttributes(const id_t textureId, const std::vector<attributeAndValue>& attributes);
/**
 * @brief Get a texture.
 *
 * @param textureId The id of the texture to return.
 * @return A pointer to the texture on success or NULL on failure. See the logs for more information.
 *
 * @see Texture
 *
 * @ingroup pg_gpu
 */
Texture* GetTexture(const id_t textureId);

} // namespace pg
