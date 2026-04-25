#include "../include/pg_audiodevice.hpp"
#include "../include/pg.hpp"

namespace pg {

AudioDevice* audioDevice;

AudioDevice::AudioDevice() {
    audioDevice = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
}

AudioDevice::~AudioDevice() {
    MIX_DestroyMixer(audioDevice);
}

bool CreateAudioDevice() {
    if (cContext->audioDevice) {
        Log(1, "AudioDevice already exists, deleting old one");
        delete cContext->audioDevice;
    }

    cContext->audioDevice = new AudioDevice;
    if (!cContext->audioDevice->audioDevice) {
        Log(2, std::format("Could not create audio device: {}", SDL_GetError()));
        return 0;
    }
    audioDevice = cContext->audioDevice;

    audioDatas  = &audioDevice->audioDatas;
    audioTracks = &audioDevice->audioTracks;

    return 1;
}

} // namespace pg
