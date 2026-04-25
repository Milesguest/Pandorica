#include "../include/pg_audiotrack.hpp"
#include "../include/pg_main.hpp"

namespace pg {

idmap<AudioTrack>* audioTracks;

AudioTrack::AudioTrack(id_t id) : EngineResource(id) {
    (*audioTracks)[id] = this;
}

AudioTrack::~AudioTrack() {
    audioTracks->erase(id);
}

bool CreateAudioTrack(const id_t audioTrackId, const id_t audioDataId) {
    if (!audioTrackId) {
        Log(1, "AudioTrackId is not allowed to be 0");
        return 0;
    }

    if (!audioDatas->contains(audioDataId)) {
        Log(1, std::format("AudioData at id {} does not exist, cannot play audio", audioDataId));
        return 0;
    }

    if (audioTracks->contains(audioTrackId)) {
        Log(1, std::format("AudioTrack at id {} already exists, deleting it", audioTrackId));
        delete audioTracks->at(audioTrackId);
    }

    AudioTrack* audioTrack = new AudioTrack(audioTrackId);

    audioTrack->audioDataId = audioDataId;
    audioTrack->audioTrack  = MIX_CreateTrack(audioDevice->audioDevice);
    if (!MIX_SetTrackAudio(audioTrack->audioTrack, audioDatas->at(audioDataId)->audioData)) {
        Log(1, std::format("Could not set track audio: {}", SDL_GetError()));
        return 0;
    }

    MIX_PlayTrack(audioTrack->audioTrack, 0);

    Log(0, std::format("Successfully created audio track at id {}", audioTrackId));
    return 1;
}

bool DeleteAudioTrack(const id_t audioTrackId) {
    if (!audioTracks->contains(audioTrackId)) {
        Log(1, std::format("Track at id {} does not exist, exiting delete function", audioTrackId));
        return 0;
    }

    AudioTrack* audioTrack = audioTracks->at(audioTrackId);

    MIX_StopTrack(audioTrack->audioTrack, 0);
    delete audioTrack;

    Log(0, std::format("Successfully deleted audioTrack at id {}", audioTrackId));
    return 1;
}

bool ContinueAudioTrack(const id_t audioTrackId) {
    if (!audioTracks->contains(audioTrackId)) {
        Log(1, std::format("Track at id {} does not exist, cannot continue it", audioTrackId));
        return 0;
    }

    if (!MIX_ResumeTrack(audioTracks->at(audioTrackId)->audioTrack)) {
        Log(1, std::format("Could not resume track: {}", SDL_GetError()));
        return 0;
    }

    Log(0, std::format("Successfully resumed track at id {}", audioTrackId));
    return 1;
}

bool PauseAudioTrack(const id_t audioTrackId) {
    if (!audioTracks->contains(audioTrackId)) {
        Log(1, std::format("Track at id {} does not exist, cannot continue it", audioTrackId));
        return 0;
    }

    if (!MIX_PauseTrack(audioTracks->at(audioTrackId)->audioTrack)) {
        Log(1, std::format("Could not resume track: {}", SDL_GetError()));
        return 0;
    }

    Log(0, std::format("Successfully paused track at id {}", audioTrackId));
    return 1;
}

AudioTrack* GetAudioTrack(const id_t audioTrackId) {
    if (!audioTracks->contains(audioTrackId)) {
        Log(1, std::format("Track at id {} does not exist, cannot get it", audioTrackId));
        return 0;
    }

    return audioTracks->at(audioTrackId);
}

} // namespace pg
