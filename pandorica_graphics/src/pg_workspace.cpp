#include "../include/pg_workspace.hpp"
#include "../include/pg.hpp"
#include "../include/pg_generic.hpp"

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

Workspace* GetWorkspace(const id_t workspaceId) {
    if (!workspaces->contains(workspaceId)) {
        Log(1, std::format("Workspace at id {} does not exist, cannot get it", workspaceId));
        return nullptr;
    }

    return workspaces->at(workspaceId);
}

} // namespace pg
