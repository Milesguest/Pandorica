#pragma once
#include "pg_generic.hpp"

/** @file */
namespace pg {
/**
 * @brief A camera creating a scene.
 *
 * A camera is a point in space that looks at another point in that space.
 * cameras are part of the window.
 *
 * @note The camera only sees enabledWorkspaces.
 * @see Workspace, CreateCamera(), DeleteCamera(), SetCameraAttributes(), GetCamera()
 *
 * @ingroup pg_window
 */
struct Camera : EngineResource {
    mat4f proj; /**< @internal The projection matrix */
    mat4f view; /**< @internal The view matrix */

    float fovY   = 70.f * M_PI / 180.f; /**< The y-axis of the field-of-view. */
    vec3f eye    = {0.f, 0.f, 1.f};     /**< The position of the camera. */
    vec3f target = {0.f, 0.f, 0.f};     /**< The point the camera is looking at */
    vec3f up     = {0.f, 1.f, 0.f};     /**< What is 'up' for the camera */
    float aspect = 1.6;                 /**< The aspect ratio of the camera */

    float near = 0.001f; /**< Near-plane (just leave it as is if you don't understand what it is). */
    float far  = 1000.f; /**< Far-plane (just leave it as is if you don't understand what it is). */

    bool changedProj = 1; /**< @internal Whether the projection matrix should be calculated again. */
    bool changedView = 1; /**< @internal Whether the view matrix should be calculated again. */
    bool ddd         = 1; /**< Whether or not the camera should be 3D or 2D. */

    std::list<id_t> enabledWorkspaces; /**< A list of workspaces the camera can 'see'. */

    bool enabled = 1; /**< Whether or not the camera should be rendered. */

    Camera(id_t id);
    ~Camera();
};
extern idmap<Camera>* cameras; /**< Pointer to 'window.cameras' */

/**
 * @brief Creates a camera.
 *
 * @note By default no workspaces are enabled. You have to manually add them.
 *
 * @param cameraId The id at which to create the camera.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Camera, DeleteCamera()
 *
 * @ingroup pg_window
 */
bool CreateCamera(const id_t cameraId);
/**
 * @brief Deletes a camera.
 *
 * @param cameraId The id of the camera to delete.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Camera, CreateCamera()
 *
 * @ingroup pg_window
 */
bool DeleteCamera(const id_t cameraId);
/**
 * @brief Set attributes of a camera.
 *
 * Supported attribute(s): \n
 * POSITION_X, R_POSITION_X, POSITION_Y, R_POSITION_Y, POSITION_Z,
 * R_POSITION_Z, TARGET_X, R_TARGET_X, TARGET_Y, R_TARGET_Y, TARGET_Z,
 * R_TARGET_Z, FOV_Y, ASPECT_RATIO, DDD, ENABLED, ADD_WORKSPACE_ID,
 * REMOVE_WORKSPACE_ID.
 *
 * @param cameraId The id of the camera of which to set the attributes for.
 * @param attributes A list of attributes and values to set.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Camera
 *
 * @ingroup pg_window
 */
bool SetCameraAttributes(const id_t cameraId, const std::vector<attributeAndValue>& attributes);
/**
 * @brief Get a camera
 *
 * @param cameraId The id of the camera to return.
 * @return A pointer to the camera of the specified id or NULL on failure.
 *
 * @see Camera
 *
 * @ingroup pg_window
 */
Camera* GetCamera(const id_t cameraId);

} // namespace pg
