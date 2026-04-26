#include "../include/pg_workspace.hpp"
#include "../include/pg.hpp"
#include "../include/pg_generic.hpp"
#include "../workspace_compiler/enums.hpp"

namespace pg {

idmap<Workspace>* workspaces;

Workspace::Workspace(id_t id) : EngineResource(id) {
    (*workspaces)[id] = this;
}
Workspace::~Workspace() {
    (*workspaces).erase(id);
}

bool CreateWorkspace(const id_t workspaceId) {
    if (!workspaceId) {
        Log(1, "WorkspaceId is not allowed to be 0");
        return 0;
    }

    if ((*workspaces).contains(workspaceId)) {
        Log(1, std::format("Workspace at id {} already exists, deleting it", workspaceId));
        delete (*workspaces)[workspaceId];
    }

    new Workspace(workspaceId);

    Log(0, std::format("Successfully created workspace at id {}", workspaceId));
    return 1;
}

bool DeleteWorkspace(const id_t workspaceId) {
    if (!window->workspaces.contains(workspaceId)) {
        Log(1, std::format("Workspace at id {} does not exist, exiting delete function", workspaceId));
        return 0;
    }

    Workspace* workspace = workspaces->at(workspaceId);

    for (TransferBuffer* transferBuffer : workspace->transferBuffers)
        DeleteTransferBuffer(transferBuffer->id);
    for (AudioData* audioData : workspace->audioDatas)
        DeleteAudioData(audioData->id);

    delete window->workspaces[workspaceId];

    Log(0, std::format("Successfully deleted workspace at id {}", workspaceId));
    return 1;
}

bool SetWorkspaceAttributes(const id_t workspaceId, std::vector<attributeAndValue>& attributes) {
    if (!workspaces->contains(workspaceId)) {
        Log(1, std::format("Workspace at id {} does not exist, cannot set its attributes", workspaceId));
        return 0;
    }

    Workspace* workspace = workspaces->at(workspaceId);

    bool error = 1;

    for (auto i : attributes) {
        switch (i.first) {
        case ENABLED:
            workspace->enabled = i.second.asUint();
            break;
        default:
            Log(1, "This attribute is not available for workspaces");
            error = 0;
            break;
        }
    }

    Log(0, std::format("Successfully set attributes for workspace at id {}", workspaceId));
    return error;
}

inline bool CreateCommand(uint8_t command, std::vector<std::pair<ws::Attr, GenericValue32>>& attributes, std::string& path, id_t& texId, id_t& tbId, id_t& wsId) {
    std::vector<attributeAndValue> setAttributes{};

    bool error = 1;

    id_t    id      = 0;
    uint8_t attCode = 0;
    size_t  size;
    id_t    fontId    = 0;
    id_t    specialId = 0;

    for (std::pair<ws::Attr, GenericValue32> attribute : attributes) {
        switch (attribute.first) {
        case ws::Attr::ID:
            id = attribute.second.asUint();
            continue;
        case ws::Attr::BSIZE:
            size = attribute.second.asUint();
            continue;
        case ws::Attr::TEXID:
            if (command == 'x')
                fontId = attribute.second.asUint();
            if (command == 'o')
                specialId = attribute.second.asUint();
            continue;
        case ws::Attr::TBID:
            if (command == 't' || command == 'x')
                specialId = attribute.second.asUint();
            continue;
        case ws::Attr::WSID:
            if (command == 'r' || command == 'a')
                specialId = attribute.second.asUint();
            continue;
        default:
            attCode = ws::GetPG(attribute.first);
            setAttributes.push_back({static_cast<Attribute>(attCode), attribute.second});
            continue;
        }
    }
    switch (command) {
    case 1:
        if (!id)
            id = GetLowestKey(objects, settings::START_SYSTEM_RESOURCES);
        if (!specialId)
            specialId = texId;

        error = CreateObject(id, specialId);
        if (!error)
            return 0;
        error = SetObjectAttributes(id, setAttributes);
        break;
    case 2:
        if (!id)
            id = GetLowestKey(textures, settings::START_SYSTEM_RESOURCES);
        if (!specialId)
            specialId = tbId;

        error = CreateTexture(id, specialId, path);
        if (!error)
            return 0;
        texId = id;
        error = SetTextureAttributes(id, setAttributes);
        break;
    case 3:
        if (!id)
            id = GetLowestKey(cameras, settings::START_SYSTEM_RESOURCES);
        error = CreateCamera(id);
        if (!error)
            return 0;
        error = SetCameraAttributes(id, setAttributes);
        break;
    case 4:
        if (!id)
            id = GetLowestKey(workspaces, settings::START_SYSTEM_RESOURCES);
        error = CreateWorkspace(id);
        if (!error)
            return 0;
        wsId  = id;
        error = SetWorkspaceAttributes(id, setAttributes);
        break;
    case 5:
        if (!id)
            id = GetLowestKey(transferBuffers, settings::START_SYSTEM_RESOURCES);
        error = CreateTransferBuffer(id, {.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = static_cast<Uint32>(size)});
        if (!error)
            return 0;
        tbId = id;
        if (!specialId)
            specialId = wsId;
        setAttributes.push_back({WORKSPACE_ID, specialId});
        error = SetTransferBufferAttributes(id, setAttributes);
        break;
    case 6:
        if (!id)
            id = GetLowestKey(buffers, settings::START_SYSTEM_RESOURCES);
        error = CreateBuffer(id, {.usage = SDL_GPU_BUFFERUSAGE_VERTEX, .size = static_cast<Uint32>(size)});
        if (!error)
            return 0;
        error = SetBufferAttributes(id, setAttributes);
        break;
    case 7:
        if (!id)
            id = GetLowestKey(audioDatas, settings::START_SYSTEM_RESOURCES);
        error = CreateAudioData(id, path);
        if (!error)
            return 0;
        if (!specialId)
            specialId = wsId;
        setAttributes.push_back({WORKSPACE_ID, specialId});
        error = SetAudioDataAttributes(id, setAttributes);
        break;
    case 8:
        if (!id)
            id = GetLowestKey(texts, settings::START_SYSTEM_RESOURCES);
        if (!specialId)
            specialId = tbId;
        error = CreateText(id, fontId, specialId);
        if (!error)
            return 0;
        error = SetTextAttributes(id, setAttributes);
        break;
    default:
        Log(1, "Unrecognized command");
        return 0;
        break;
    }

    attributes.clear();

    return error;
}

bool LoadWorkspace(const std::string& path) {
    if (!std::filesystem::exists(std::format("{}/{}", cContext->basePath, path))) {
        Log(1, std::format("File at {} does not exist, cannot load workspace", path));
        return 0;
    }

    bool error = 1;

    std::ifstream in(std::format("{}/{}", cContext->basePath, path), std::ios::binary);

    cereal::BinaryInputArchive archive(in);

    id_t tbId  = 0;
    id_t texId = 0;
    id_t wsId  = 0;

    GenericValue32 value;

    bool    eof     = 0;
    uint8_t command = 0;
    uint8_t attName = 0;
    uint8_t letter  = 0;

    std::vector<std::pair<ws::Attr, GenericValue32>> attributes;

    std::string valuePath;

    while (!eof) {
        in.peek();
        if (in.eof()) {
            eof = 1;
            if (error)
                error = CreateCommand(command, attributes, valuePath, texId, tbId, wsId);
            else
                CreateCommand(command, attributes, valuePath, texId, tbId, wsId);
            in.close();
            break;
        }

        archive(letter);

        if (letter < 0xFF) {
            if (command) {
                if (error)
                    error = CreateCommand(command, attributes, valuePath, texId, tbId, wsId);
                else
                    CreateCommand(command, attributes, valuePath, texId, tbId, wsId);
            }
            command = letter;
            continue;
        }

        archive(attName);
        if (!attName) {
            archive(valuePath);
        } else {
            archive(value);
            attributes.push_back({static_cast<ws::Attr>(attName), value});
        }
    }
    return error;
}

Workspace* GetWorkspace(const id_t workspaceId) {
    if (!workspaces->contains(workspaceId)) {
        Log(1, std::format("Workspace at id {} does not exist, cannot get it", workspaceId));
        return nullptr;
    }

    return workspaces->at(workspaceId);
}

} // namespace pg
