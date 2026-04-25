#pragma once
#include "pg_generic.hpp"
#include "pg_texture.hpp"

/** @file */
namespace pg {

struct Texture;
/**
 * @brief An object containing data to draw.
 *
 * All objects are 2D plains. They can be resized, rotated and moved but will always be a 2D plain.
 *
 * An object is part of the window and referenced by textures.
 *
 * @see Texture, CreateObject(), DeleteObject(), SetObjectAttributes(), GetObject()
 *
 * @ingroup pg_window
 */
struct Object : EngineResource {
    std::shared_ptr<Instance> instance; /**< Pointer to the Instance that will be uploaded to the GPU. Can be shared with @ref Text. */

    vec3f rotation     = {0.0f, 0.0f, 0.0f};       /**< The rotation of the object. */
    bool  rotated      = 1;                        /**< @internal Whether or not the object was rotated. */
    vec4i color        = {0xFF, 0xFF, 0xFF, 0xFF}; /**< The color of the object (R8G8B8A8) */
    bool  colorChanged = 1;                        /**< Whether or not the object color changed. */

    bool enabled = 1; /**< Whether or not the object should be drawn. Only updates when uploaded. */

    Texture* texture     = nullptr; /**< Pointer to the texture with which the object should be drawn. */
    id_t     workspaceId = 0;       /**< The id of the workspace the object is assigned to. */

    Object(id_t id);
    Object(id_t id, id_t textureId);
    Object(id_t id, id_t textureId, std::shared_ptr<Instance> instance);
    ~Object();

    /** @internal */
    template <class Archive>
    void serialize(Archive& archive);
};
extern idmap<Object>* objects; /**< Pointer to 'window->objects'. */

/**
 * @brief Creates an object.
 *
 * Creates an object at the specified id with the specified texture.
 *
 * @param objectId The id at which to create the object.
 * @param textureId the id of the texture the object should use.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Object, DeleteObject()
 *
 * @ingroup pg_window
 */
bool CreateObject(const id_t objectId, const id_t textureId);
/**
 * @brief Deletes an object.
 *
 * Deletes the object at the specified id.
 *
 * @param objectId The id of the object to delete.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Object, CreateObject()
 *
 * @ingroup pg_window
 */
bool DeleteObject(const id_t objectId);
/**
 * @brief Set the attributes for an object.
 *
 * Supported attribute(s): \n
 * POSITION_X, R_POSITION_X, POSITION_Y, R_POSITION_Y, POSITION_Z, R_POSITION_Z, WIDTH, R_WIDTH, HEIGHT, R_HEIGHT, CUT_X, R_CUT_X, CUT_Y, R_CUT_Y, CUT_WIDTH, R_CUT_WIDTH, CUT_HEIGHT, R_CUT_HEIGHT, ORIGIN_X, R_ORIGIN_X, ORIGIN_Y, R_ORIGIN_Y, ORIGIN_Z, R_ORIGIN_Z, ROTATION_X, R_ROTATION_X, ROTATION_Y, R_ROTATION_Y, ROTATION_Z, R_ROTATION_Z, RED, R_RED, GREEN, R_GREEN, BLUE, R_BLUE, ALPHA, R_ALPHA, ENABLED, TEXTURE_ID
 *
 * @param objectId The id of the object which attributes should be changed.
 * @param attributes A vector of attributes to change and values.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Object
 *
 * @ingroup pg_window
 */

bool SetObjectAttributes(const id_t objectId, const std::vector<attributeAndValue>& attributes);
/**
 * @overload bool SetObjectAttributes(const id_t objectId, const std::vector<attributeAndValue>& attributes)
 * @brief Change the attributes of objects.
 *
 * @param objectIds A vector of object ids to change.
 * @param attributes A vector of attributes to change and values.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Object
 *
 * @ingroup pg_window
 */
bool SetObjectsAttributes(const std::vector<id_t> objectIds, const std::vector<attributeAndValue>& attributes);
/**
 * @brief Get an object.
 *
 * @param objectId The id of the object to return.
 * @return A pointer to the object on success or NULL on failure. See the logs for more information.
 *
 * @see Object
 *
 * @ingroup pg_window
 */
Object* GetObject(const id_t objectId);

} // namespace pg
