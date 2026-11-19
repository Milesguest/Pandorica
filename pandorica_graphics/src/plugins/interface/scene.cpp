#include "../../../include/plugins/interface/scene.hpp"

namespace pg::interface {

pg::idmap<Scene>* scenes;

Scene::Scene(id_t sceneId) : sceneId(sceneId) {
    (*scenes)[sceneId] = this;
}

Scene::~Scene() {
    while (!this->buttons.empty())
        delete buttons.back();
    while (!this->textFields.empty())
        delete textFields.back();

    scenes->erase(sceneId);
}

bool CreateScene(id_t sceneId) {
    if (!sceneId) {
        pg::Log(1, "SceneId is not allowed to be 0");
        return 0;
    }

    if (scenes->contains(sceneId)) {
        pg::Log(1, std::format("Scene at id {} already exists. Deleting it", sceneId));
        delete scenes->at(sceneId);
    }

    Scene* scene = new Scene(sceneId);

    pg::Log(0, std::format("Successfully created scene at id {}", sceneId));
    return 1;
}

bool DeleteScene(id_t sceneId) {
    if (!scenes->contains(sceneId)) {
        pg::Log(1, std::format("Cannot delete scene at id {} as it does not exist.", sceneId));
        return 0;
    }

    delete scenes->at(sceneId);

    pg::Log(0, std::format("Successfully deleted scene at id {}", sceneId));
    return 1;
}
bool SetSceneAttributes(id_t sceneId, std::vector<std::pair<Attribute, pg::GenericValue32>> attributes) {
    if (!scenes->contains(sceneId)) {
        pg::Log(1, std::format("Scene at id {} does not exist. Cannot set its attributes", sceneId));
        return 0;
    }

    Scene* scene = scenes->at(sceneId);
    bool   error = 1;

    for (auto i : attributes)
        switch (i.first) {
        case pg::ENABLED:
            scene->enabled = i.second.asBool();
            break;
        default:
            pg::Log(1, "Attribute is unavailable for type scene.");
            error = 0;
            break;
        }

    pg::Log(0, std::format("Succesfully set attributes for scene at id {}", sceneId));
    return error;
}

Scene* GetScene(id_t sceneId) {
    if (!scenes->contains(sceneId)) {
        pg::Log(1, std::format("Scene at id {} does not exist, cannot get it", sceneId));
        return nullptr;
    }

    pg::Log(0, std::format("Successfully got scene at id {}", sceneId));
    return scenes->at(sceneId);
}

} // namespace pg::interface
