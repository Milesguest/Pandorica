#pragma once
#include "pg_generic.hpp"

/** @file */
namespace pg {
/**
 * @brief Struct containing raw audio data and metadata for it.
 *
 * AudioData contains raw audio data as MIX_Audio (SDL3_Mixer).
 * It can be assigned to a workspace.
 *
 * Audio data is part of the AudioDevice an is referenced by audio tracks.
 *
 * @see AudioDevice, AudioTrack, CreateAudioData(), DeleteAudioData(), SetAudioDataAttributes(), GetAudioData()
 * @ingroup pg_audio
 */
struct AudioData : EngineResource {
    MIX_Audio* audioData; /**< The pointer to the raw audio data. */

    std::string path; /**< The path from which the audioData was created. */

    id_t workspaceId = 0; /**< The id of the workspace it is assigned to. */
    /**
     * @brief A list of audio tracks created with this data.
     * @note Audio tracks will not stop playing if their audio
     * data is deleted.
     */
    std::list<id_t> audioTrackIds;

    AudioData(id_t id);
    ~AudioData();

    /**
     * @internal Logic for serialization.
     */
    template <class Archive>
    void serialize(Archive& archive) {
        archive(path);
    }
};
extern idmap<AudioData>* audioDatas; /**< @brief Pointer to 'audioDevice.audioDatas' */

/**
 * @brief Load audio data from a file.
 *
 * This creates audio data from the specified path.
 * Supported file formats can be acquired from the 'SDL3_mixer' wiki.
 *
 * @param audioDataId The id for the newly created audio data.
 * @param path The path from which to load the audio data.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see AudioData, DeleteAudioData()
 * @ingroup pg_audio
 */
bool CreateAudioData(const id_t audioDataId, const std::string& path);
/**
 * @overload bool CreateAudioData(const id_t audioDataId, const std::string& path)
 *
 * @param audioDataId The id for the newly created audio data.
 * @param path The path from which to load the audio data.
 * @param workspaceId The workspaceId to assign it to.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Workspace, AudioData, DeleteAudioData()
 *
 * @ingroup pg_audio
 */
bool CreateAudioData(const id_t audioDataId, const std::string& path, const id_t workspaceId);
/**
 * @brief Deletes audio data.
 *
 * @note audio tracks will not stop playing
 *
 * @param audioDataId The id of the AudioData to delete.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see AudioData, CreateAudioData()
 * @ingroup pg_audio
 */
bool DeleteAudioData(const id_t audioDataId);
/**
 * @brief Set attributes of audio data.
 *
 * Supports the attribute(s): \n
 * WORKSPACE_ID
 *
 * @param audioDataId The id of the AudioData to modify.
 * @param attributes A list of attributes and values that will be set.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see AudioData
 * @ingroup pg_audio
 */
bool SetAudioDataAttributes(const id_t audioDataId, const std::vector<attributeAndValue>& attributes);
/**
 * @brief Get the audio data of the specified id.
 *
 * @param audioDataId The id of the AudioData to return.
 * @return The audio data of the specified id or NULL if it could not be found.
 *
 * @see AudioData
 * @ingroup pg_audio
 */
AudioData* GetAudioData(const id_t audioDataId);

} // namespace pg
