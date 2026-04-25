#include "../include/pg_audiodata.hpp"
#include "../include/pg_main.hpp"

namespace pg {

idmap<AudioData>* audioDatas;

AudioData::AudioData(id_t id) : EngineResource(id) {
    (*audioDatas)[id] = this;
}

AudioData::~AudioData() {
    audioDatas->erase(id);
}

bool CreateAudioData(const id_t audioDataId, const std::string& path) {
    if (!audioDataId) {
        Log(1, "AudioDataId is not allowed to be 0");
        return 0;
    }

    std::string fullPath;

    if (!std::filesystem::exists(fullPath = std::format("{}/{}", cContext->basePath, path))) {
        Log(1, std::format("File at {} does not exist, cannot load audio", path));
        return 0;
    }

    if (audioDatas->contains(audioDataId)) {
        Log(1, std::format("AudioData at id {} already exists, deleting it", audioDataId));
        delete audioDatas->at(audioDataId);
    }

    AudioData* audioData = new AudioData(audioDataId);

    audioData->audioData = MIX_LoadAudio(audioDevice->audioDevice, fullPath.c_str(), 1);
    if (!audioData->audioData) {
        Log(1, std::format("Could not load audio data from {}", path));
        delete audioData;
        return 0;
    }
    audioData->path = path;

    Log(0, std::format("Successfully loaded audio data from {} to id {}", path, audioDataId));
    return 1;
}

bool CreateAudioData(const id_t audioDataId, const std::string& path, const id_t workspaceId) {
    if (!workspaces->contains(workspaceId)) {
        Log(1, std::format("Workspace at id {} does not exist, cannot create audio data", workspaceId));
        return 0;
    }

    if (!CreateAudioData(audioDataId, path)) return 0;

    AudioData* audioData;
    workspaces->at(workspaceId)->audioDatas.push_back(audioData = audioDatas->at(audioDataId));
    audioData->workspaceId = workspaceId;

    Log(0, std::format("Successfully assigned audio data at id {} to workspace at id {}", audioDataId, workspaceId));
    return 1;
}

bool DeleteAudioData(const id_t audioDataId) {
    if (!audioDatas->contains(audioDataId)) {
        Log(1, std::format("AudioData at id {} does not exist, exiting delete function", audioDataId));
        return 0;
    }

    delete audioDatas->at(audioDataId);

    Log(0, std::format("Successfully deleted audioData at id {}", audioDataId));
    return 1;
}

bool SetAudioDataAttributes(const id_t audioDataId, const std::vector<attributeAndValue>& attributes) {
    if (!audioDatas->contains(audioDataId)) {
        Log(1, std::format("AudioData at id {} does not exist, cannot set it's attributes.", audioDataId));
        return 0;
    }
    AudioData* audioData = audioDatas->at(audioDataId);

    bool error = 1;

    for (attributeAndValue attribute : attributes) {
        switch (attribute.first) {
        case WORKSPACE_ID:
            if (!workspaces->contains(attribute.second.asUint())) {
                Log(1, std::format("Workspace at id {} does not exist, cannot set is as attribute", attribute.second.asUint()));
                error = 0;
                continue;
            }
            workspaces->at(attribute.second.asUint())->audioDatas.push_back(audioData);
            audioData->workspaceId = attribute.second.asUint();

            break;
        default:
            Log(1, "Attribute not supported for audioData");
            error = 0;
            continue;
        }
    }

    Log(0, std::format("Successfully set attributes for audioData at id {}", audioDataId));
    return error;
};

AudioData* GetAudioData(const id_t audioDataId) {
    if (!audioDatas->contains(audioDataId)) {
        Log(1, std::format("AudioData at id {} does not exist, cannot get it", audioDataId));
        return nullptr;
    }

    return audioDatas->at(audioDataId);
}

} // namespace pg
