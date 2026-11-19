#pragma once

#include "button.hpp"
#include "generic.hpp"
#include "textField.hpp"

/** @file */
namespace pg {
namespace interface {

struct Scene {
    const id_t sceneId;

    pg::vector<Button*>    buttons{};
    pg::vector<TextField*> textFields{};

    bool enabled = 1;

    Scene(id_t sceneId);
    ~Scene();
};
extern pg::idmap<Scene>* scenes;

bool   CreateScene(id_t sceneId);
bool   DeleteScene(id_t sceneId);
bool   SetSceneAttributes(id_t sceneId, std::vector<std::pair<Attribute, pg::GenericValue32>> attributes);
Scene* GetScene(id_t sceneId);

} // namespace interface
} // namespace pg
