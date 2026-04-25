#include "../include/pg_camera.hpp"
#include "../include/pg.hpp"

namespace pg {

idmap<Camera>* cameras;

Camera::Camera(id_t id) : EngineResource(id) {
    (*cameras)[id] = this;
}
Camera::~Camera() {
    cameras->erase(id);
}

bool CreateCamera(const id_t cameraId) {
    if (!cameraId) {
        Log(1, "CameraId is not allowed to be 0");
        return 0;
    }
    if (cameras->contains(cameraId)) {
        Log(1, std::format("Camera at id {} already exists, deleting it", cameraId));
        delete (*cameras)[cameraId];
    }

    new Camera(cameraId);

    Log(0, std::format("Successfully created camera at id {}", cameraId));
    return 1;
}

bool DeleteCamera(const id_t cameraId) {
    if (!cameras->contains(cameraId)) {
        Log(1, std::format("Camera at id {} does not exist, exiting delete function", cameraId));
        return 0;
    }

    delete (*cameras)[cameraId];

    Log(0, std::format("Successfully deleted camera at id {}", cameraId));
    return 1;
}

bool SetCameraAttributes(const id_t cameraId, const std::vector<attributeAndValue>& attributes) {
    if (!cameras->contains(cameraId)) {
        Log(1, std::format("Camera at id {} does not exist, exiting delete function", cameraId));
        return 0;
    }

    Camera* camera = cameras->at(cameraId);

    bool error = 1;

    for (auto i : attributes) {
        switch (i.first) {
        case POSITION_X:
            camera->eye.x()     = i.second.asFloat();
            camera->changedView = 1;
            break;
        case R_POSITION_X:
            camera->eye.x() += i.second.asFloat();
            camera->changedView = 1;
            break;
        case POSITION_Y:
            camera->eye.y()     = i.second.asFloat();
            camera->changedView = 1;
            break;
        case R_POSITION_Y:
            camera->eye.y() += i.second.asFloat();
            camera->changedView = 1;
            break;
        case POSITION_Z:
            camera->eye.z()     = i.second.asFloat();
            camera->changedView = 1;
            break;
        case R_POSITION_Z:
            camera->eye.z() += i.second.asFloat();
            camera->changedView = 1;
            break;
        case TARGET_X:
            camera->target.x()  = i.second.asFloat();
            camera->changedView = 1;
            break;
        case R_TARGET_X:
            camera->target.x() += i.second.asFloat();
            camera->changedView = 1;
            break;
        case TARGET_Y:
            camera->target.y()  = i.second.asFloat();
            camera->changedView = 1;
            break;
        case R_TARGET_Y:
            camera->target.y() += i.second.asFloat();
            camera->changedView = 1;
            break;
        case TARGET_Z:
            camera->target.z()  = i.second.asFloat();
            camera->changedView = 1;
            break;
        case R_TARGET_Z:
            camera->target.z() += i.second.asFloat();
            camera->changedView = 1;
            break;
        case FOV_Y:
            camera->fovY        = i.second.asFloat() * M_PI / 180.f;
            camera->changedProj = 1;
            break;
        case ASPECT_RATIO:
            camera->aspect      = i.second.asFloat();
            camera->changedProj = 1;
            break;
        case DDD:
            camera->ddd         = i.second.asBool();
            camera->changedProj = 1;
            break;
        case ENABLED:
            camera->enabled = i.second.asBool();
            break;
        case ADD_WORKSPACE_ID:
            if (!(*workspaces).contains(i.second.asUint())) {
                Log(1, std::format("Workspace at id {} does not exist, cannot enable it", static_cast<id_t>(i.second)));
                error = 0;
                break;
            }
            camera->enabledWorkspaces.push_back(i.second.asUint());
            break;
        case REMOVE_WORKSPACE_ID:
            if (!camera->enabledWorkspaces.remove(i.second.asUint())) {
                Log(1, "This workspace was never enabled for this camera, cannot remove it");
                error = 0;
            }
            break;
        default:
            Log(1, "This attribute is not supported for cameras");
            error = 0;
            break;
        }
    }

    Log(0, std::format("Successfully set attributes for camera at id {}", cameraId));
    return error;
}

Camera* GetCamera(const id_t cameraId) {
    if (!cameras->contains(cameraId)) {
        Log(1, std::format("Camera at id {} does not exist, cannot get it", cameraId));
        return nullptr;
    }

    return (*cameras)[cameraId];
}

} // namespace pg
