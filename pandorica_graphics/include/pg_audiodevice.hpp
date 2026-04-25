#pragma once
#include "pg_audiodata.hpp"
#include "pg_audiotrack.hpp"
#include "pg_generic.hpp"

/** @file */
namespace pg {
/**
 * @brief The audio device used to play and change audio.
 *
 * An AudioDevice is used to create audio streams and play
 * audio.
 *
 * @see Context, AudioData, AudioTrack, CreateAudioDevice()
 * @ingroup pg_audio
 */
struct AudioDevice {
    /**
     * Object used for interaction with the audio devices.
     *
     * @note Use SDL3_mixer to change specific parts of audios.
     */
    MIX_Mixer* audioDevice;

    idmap<AudioData>  audioDatas{};  /**< Map of all audio data created. */
    idmap<AudioTrack> audioTracks{}; /**< Map of all audio tracks. */

    AudioDevice();
    ~AudioDevice();
};
extern AudioDevice* audioDevice; /**< @brief Pointer to the created audio device. */

/**
 * @brief Creates the audio device.
 *
 * Create the audio device. You must run 'pg::Quit' to destroy it or manually delete it.
 * You must create the audio device before creating audio data or tracks.
 *
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see AudioDevice
 * @ingroup pg_audio
 */
bool CreateAudioDevice();

} // namespace pg
