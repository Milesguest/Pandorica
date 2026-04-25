#pragma once
#include "pg_audiodata.hpp"
#include "pg_generic.hpp"
#include "pg_transferbuffer.hpp"

/** @file */
namespace pg {

/**
 * @brief Workspace grouping resources.
 *
 * A workspace is used to group resources into logical groups. They can be created with the 'workspace_compiler' or manually through functions.
 *
 * Workspaces are part of the window and referenced by transferbuffers, textures, objects, audio data and texts.
 *
 * @note To render workspaces you have to enable them for a specific camera first.
 *
 * @see TransferBuffer, AudioData, Window
 *
 * @ingroup pg_window
 */
struct Workspace : EngineResource {
    std::list<TransferBuffer*> transferBuffers{}; /**< List of all assigned transferBuffers. */
    std::list<AudioData*>      audioDatas{};      /**< List of all assigned audio data */

    bool enabled = 1; /**< Whether or not the workspace should be drawn. */

    Workspace(id_t id);
    ~Workspace();
};
extern idmap<Workspace>* workspaces; /**< Pointer to 'window->workspaces'. */

/**
 * @brief Creates a workspace.
 *
 * @param workspaceId The id at which to create the workspace.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Workspace, DeleteWorkspace()
 *
 * @ingroup pg_window
 */
bool CreateWorkspace(const id_t workspaceId);
/**
 * @brief Deletes a workspace.
 *
 * @note All transferbuffers (and thus also textures, texts and objects) and audio data assigned to this workspace will also be deleted.
 *
 * @param workspaceId The id of the workspace to delete.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Workspace
 *
 * @ingroup pg_window
 */
bool DeleteWorkspace(const id_t workspaceId);
/**
 * @brief Load a workspace(s) from a binary workspace file (.bwf)
 *
 * Loads the workspace(s) from the file into the programm.
 * See @ref example.twf for more information.
 *
 * @param path The path at which to find the .bwf
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Workspace, DeleteWorkspace()
 *
 * @ingroup pg_window
 */
bool LoadWorkspace(const std::string& path);
/**
 * @brief Get a workspace.
 *
 * @param workspaceId The id of the workspace to return.
 * @return A pointer to the workspace on success or NULL on failure. See the logs for more information.
 *
 * @see Workspace
 *
 * @ingroup pg_window
 */
Workspace* GetWorkspace(const id_t workspaceId);

} // namespace pg
