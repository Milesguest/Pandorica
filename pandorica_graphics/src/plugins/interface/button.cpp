#include "../../../include/plugins/interface/button.hpp"
#include "../../../include/plugins/interface/scene.hpp"
#include "../../../include/plugins/interface/system.hpp"

namespace pg::interface {

pg::idmap<Button>*                          buttons;
std::unordered_map<std::string, void (*)()> buttonFunctions;

Button::Button(id_t buttonId) : buttonId(buttonId) {
    (*buttons)[buttonId] = this;
}
Button::~Button() {
    if (sceneId) {
        scenes->at(sceneId)->buttons.remove(this);
    }

    buttons->erase(buttonId);
}

bool CreateButton(id_t buttonId) {
    if (!buttonId) {
        pg::Log(1, "ButtonId is not allowed to be 0, cannot create the button");
        return 0;
    }

    if (buttons->contains(buttonId)) {
        pg::Log(1, std::format("Button at id {} already exists, deleting it.", buttonId));
        DeleteButton(buttonId);
    }

    Button* button = new Button(buttonId);

    pg::Log(0, std::format("Successfully created button at id {}", buttonId));
    return 1;
}

bool DeleteButton(id_t buttonId) {
    if (!buttons->contains(buttonId)) {
        pg::Log(1, std::format("Button at id {} does not exist, cannot delete it", buttonId));
        return 0;
    }

    delete buttons->at(buttonId);

    pg::Log(0, std::format("Successfully deleted button at id {}", buttonId));
    return 1;
}

bool SetButtonAttributes(id_t buttonId, const std::vector<std::pair<interface::Attribute, std::variant<pg::GenericValue32, const char*>>>& attributes) {
    if (!buttons->contains(buttonId)) {
        pg::Log(1, std::format("Could not set attributes of button at id {}", buttonId));
        return 0;
    }

    Button*     button = buttons->at(buttonId);
    bool        error  = 1;
    std::string funcName;
    id_t        sceneId;

    for (auto i : attributes)
        switch (i.first) {
        case pg::ENABLED:
            button->enabled = std::get<pg::GenericValue32>(i.second).asBool();
            break;
        case pg::POSITION_X:
            button->position.x() = std::get<pg::GenericValue32>(i.second).asFloat();
            break;
        case pg::R_POSITION_X:
            button->position.x() += std::get<pg::GenericValue32>(i.second).asFloat();
            break;
        case pg::POSITION_Y:
            button->position.y() = std::get<pg::GenericValue32>(i.second).asFloat();
            break;
        case pg::R_POSITION_Y:
            button->position.y() += std::get<pg::GenericValue32>(i.second).asFloat();
            break;
        case pg::WIDTH:
            button->size.x() = std::get<pg::GenericValue32>(i.second).asFloat();
            break;
        case pg::R_WIDTH:
            button->size.x() += std::get<pg::GenericValue32>(i.second).asFloat();
            break;
        case pg::HEIGHT:
            button->size.y() = std::get<pg::GenericValue32>(i.second).asFloat();
            break;
        case pg::R_HEIGHT:
            button->size.y() += std::get<pg::GenericValue32>(i.second).asFloat();
            break;
        case pg::OBJECT_ID: {
            id_t oId = std::get<pg::GenericValue32>(i.second).asUint();
            if (!pg::GetObject(oId)) {
                pg::Log(1, std::format("Object at id {} does not exist, cannot set it as attribute for button", oId));
                break;
            }

            button->objectId = oId;
            break;
        }
        case KEY_DOWN_FUNC:
            funcName = std::get<const char*>(i.second);

            if (funcName.empty()) {
                pg::Log(1, std::format("Function name is empty, cannot assign it to the button at id {}", buttonId));
                error = 0;
                break;
            }

            if (!buttonFunctions.contains(funcName)) {
                pg::Log(1, std::format("Function with name {} does not exist, cannot assign it to button.", funcName));
                error = 0;
                break;
            }

            button->keyDownFuncPtr = funcName;
            break;
        case KEY_UP_FUNC:
            funcName = std::get<const char*>(i.second);

            if (funcName.empty()) {
                pg::Log(1, std::format("Function name is empty, cannot assign it to the button at id {}", buttonId));
                error = 0;
                break;
            }

            if (!buttonFunctions.contains(funcName)) {
                pg::Log(1, std::format("Function with name {} does not exist, cannot assign it to button.", funcName));
                error = 0;
                break;
            }

            button->keyUpFuncPtr = funcName;
            break;
        case HOVER_DOWN_FUNC:
            funcName = std::get<const char*>(i.second);

            if (funcName.empty()) {
                pg::Log(1, std::format("Function name is empty, cannot assign it to the button at id {}", buttonId));
                error = 0;
                break;
            }

            if (!buttonFunctions.contains(funcName)) {
                pg::Log(1, std::format("Function with name {} does not exist, cannot assign it to button.", funcName));
                error = 0;
                break;
            }

            button->hoverDownFuncPtr = funcName;
            break;
        case HOVER_UP_FUNC:
            funcName = std::get<const char*>(i.second);

            if (funcName.empty()) {
                pg::Log(1, std::format("Function name is empty, cannot assign it to the button at id {}", buttonId));
                error = 0;
                break;
            }

            if (!buttonFunctions.contains(funcName)) {
                pg::Log(1, std::format("Function with name {} does not exist, cannot assign it to button.", funcName));
                error = 0;
                break;
            }

            button->hoverUpFuncPtr = funcName;
            break;
        case SCENE_ID:
            sceneId = std::get<pg::GenericValue32>(i.second).asUint();
            if (!scenes->contains(sceneId)) {
                pg::Log(1, std::format("Scene at id {} does not exist. Cannot set it as attribute for button", sceneId));
                error = 0;
                break;
            }
            button->sceneId = sceneId;
            scenes->at(sceneId)->buttons.push_back(button);
            break;
        default:
            pg::Log(1, "Attribute is unavailable for type button.");
            error = 0;
            break;
        }

    pg::Log(0, std::format("Succesfully set attributes for button at id {}", buttonId));
    return error;
}

Button* GetButton(id_t buttonId) {
    if (!buttons->contains(buttonId)) {
        pg::Log(1, std::format("Button at id {} does not exist, cannot get it", buttonId));
        return nullptr;
    }

    pg::Log(0, std::format("Successfully got button at id {}", buttonId));
    return buttons->at(buttonId);
}

} // namespace pg::interface
