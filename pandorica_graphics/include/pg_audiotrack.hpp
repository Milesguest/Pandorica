#pragma once
#include "pg_audiodata.hpp"
#include "pg_generic.hpp"

/** @file */
namespace pg {
/**
 * @brief An audio track playing audio.
 *
 * An audio track has to be created from audio data.
 *
 * An audio track is part of the audio device and is referenced by audio data.
 *
 * @see AudioData, AudioDevice, CreateAudioTrack(), DeleteAudioTrack(), ContinueAudioTrack(), PauseAudioTrack(), GetAudioTrack()
 * @ingroup pg_audio
 */
struct AudioTrack : EngineResource {
    /**
     * This object represents the audio track.
     *
     * @note Use this to change specific aspects of the audio track.
     */
    MIX_Track* audioTrack;

    id_t audioDataId; /**< The id of the audio track */

    AudioTrack(id_t id);
    ~AudioTrack();
};
extern idmap<AudioTrack>* audioTracks; /**< Pointer to 'audioDevice.audioTracks' */

/**
 * @brief Creates and plays an audio track.
 *
 * @param audioTrackId The id of the newly created audio track.
 * @param audioDataId The id of the audio data from which to create the audio track.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see AudioTrack, DeleteAudioTrack()
 *
 * @ingroup pg_audio
 */
bool CreateAudioTrack(const id_t audioTrackId, const id_t audioDataId);
/**
 * @brief Deletes and stops and audio track.
 *
 * @param audioTrackId The id of the audio track to delete.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see AudioTrack, CreateAudioTrack()
 *
 * @ingroup pg_audio
 */
bool DeleteAudioTrack(const id_t audioTrackId);
/**
 * @brief Continue an audio track.
 * @note Continuing an audio track which was not paused does nothing.
 *
 * @param audioTrackId The id of the audio track to continue.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see AudioTrack, PauseAudioTrack()
 *
 * @ingroup pg_audio
 */
bool ContinueAudioTrack(const id_t audioTrackId);
/**
 * @brief Pause an audio track.
 * @note Pausing an audio track which already is paused does nothing.
 *
 * @param audioTrackId The id of the audio track to pause.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see AudioTrack, ContinueAudioTrack()
 *
 * @ingroup pg_audio
 */
bool PauseAudioTrack(const id_t audioTrackId);
/**
 * @brief Get an audio track.
 *
 * @param audioTrackId The if of the audio track to return.
 * @return A pointer to the audio track if it succeeded, NULL if it could not be found.
 *
 * @see AudioTrack
 *
 * @ingroup pg_audio
 */
AudioTrack* GetAudioTrack(const id_t audioTrackId);

} // namespace pg
