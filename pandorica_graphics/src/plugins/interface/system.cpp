#include "../../../include/plugins/interface/system.hpp"
#include "../../../include/plugins/interface/button.hpp"
#include "../../../include/plugins/interface/scene.hpp"
#include "../../../include/plugins/interface/textField.hpp"
#include "../../../include/plugins/interface/textureAtlas.hpp"

namespace pg::interface {

System* cSystem;

void TextFieldDown() {
    Button* clickedButton = GetButton(cSystem->clickedButtonId);
    if (!clickedButton) {
        return;
    }

    TextField* textField = nullptr;

    for (auto i : *textFields) {
        if (i.second->buttonId == cSystem->clickedButtonId && i.second->enabled && i.second->writable) {
            textField = i.second;
            break;
        }
    }
    if (!textField) {
        pg::Log(1, std::format("No textField assigned to button at id {}. Function will exit.", cSystem->clickedButtonId));
        return;
    }

    if (!pg::texts->contains(textField->textId)) {
        pg::Log(1, std::format("Text at id {} does not exist, cannot start textField input for {}.", textField->textId, textField->textFieldId));
        return;
    }

    if (!SDL_StartTextInputWithProperties(pg::window->window, textField->textFieldInputProperties)) {
        pg::Log(1, std::format("Could not start text input: {}", SDL_GetError()));
        return;
    }
    cSystem->textFieldInputId = textField->textFieldId;
    textField->isBeingWritten = true;

    pg::Log(0, std::format("Successfully started text input for textField at id {}", textField->textFieldId));
}

void InterfaceLoop() {
    for (int i = 0; i < pg::window->events.size(); ++i) {
        SDL_Event e = pg::window->events.front();
        pg::window->events.pop();
        pg::window->events.push(e);

        switch (e.type) {
        case SDL_EVENT_MOUSE_MOTION: {
            cSystem->mousePosition.x() = static_cast<float>(e.motion.x) * 2 / pg::window->width - 1;
            cSystem->mousePosition.y() = static_cast<float>(e.motion.y) * 2 / pg::window->height - 1;
            bool foundButton           = false;

            for (auto j : *buttons) {
                Button* button = j.second;

                if (!button->enabled)
                    continue;

                Scene* scene = GetScene(button->sceneId);
                if (!scene) {
                    pg::Log(1, std::format("Scene at id {} does not exist, cannot get button scene id. Continuing with button having no scene", button->sceneId));
                } else {
                    if (!scene->enabled) {
                        continue;
                    }
                }

                // Checking if cursor is on button
                if (cSystem->mousePosition.x() <= button->position.x() / (static_cast<float>(window->width) / window->height) ||
                    cSystem->mousePosition.y() <= button->position.y() ||
                    cSystem->mousePosition.x() >= (button->position.x() + button->size.x()) / (static_cast<float>(window->width) / window->height) ||
                    cSystem->mousePosition.y() >= button->position.y() + button->size.y()) {
                    continue;
                }

                foundButton = true;
                if (cSystem->hoveredButtonId == button->buttonId) {
                    break;
                }

                // Button is in bounds, selecting it
                Button* oldHoveredButton = GetButton(cSystem->hoveredButtonId);
                if (oldHoveredButton) {
                    oldHoveredButton->isHovered = false;
                    if (!oldHoveredButton->hoverUpFuncPtr.empty()) {
                        buttonFunctions.at(oldHoveredButton->hoverUpFuncPtr)();
                    }
                }
                cSystem->hoveredButtonId = button->buttonId;
                button->isHovered        = true;
                if (!button->hoverDownFuncPtr.empty()) {
                    buttonFunctions.at(button->hoverDownFuncPtr)();
                }
                break;
            }
            if (!foundButton) {
                Button* hoveredButton = GetButton(cSystem->hoveredButtonId);
                if (hoveredButton) {
                    hoveredButton->isHovered = false;
                    if (!hoveredButton->hoverUpFuncPtr.empty()) {
                        buttonFunctions.at(hoveredButton->hoverUpFuncPtr)();
                    }
                }
                cSystem->hoveredButtonId = 0;
            }

            continue;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            Button* hoveredButton = GetButton(cSystem->hoveredButtonId);
            if (hoveredButton) {
                cSystem->clickedButtonId = hoveredButton->buttonId;
                hoveredButton->isClicked = true;
                if (!hoveredButton->keyDownFuncPtr.empty()) {
                    buttonFunctions.at(hoveredButton->keyDownFuncPtr)();
                }
            } else {
                cSystem->clickedButtonId = 0;
            }
            if (cSystem->textFieldInputId) {
                TextField* tf = GetTextField(cSystem->textFieldInputId);
                if (tf) {
                    if (tf->buttonId == cSystem->clickedButtonId)
                        continue;
                    tf->isBeingWritten = false;
                }
                cSystem->textFieldInputId = 0;
                SDL_StopTextInput(pg::window->window);
            }
            continue;
        }
        case SDL_EVENT_MOUSE_BUTTON_UP: {
            Button* clickedButton    = GetButton(cSystem->clickedButtonId);
            cSystem->clickedButtonId = 0;
            if (clickedButton) {
                clickedButton->isClicked = false;
                if (!clickedButton->keyUpFuncPtr.empty()) {
                    buttonFunctions.at(clickedButton->keyUpFuncPtr)();
                }
            }
        }
            continue;
        case SDL_EVENT_TEXT_INPUT: {
            std::string text = e.text.text;

            TextField* tf = GetTextField(cSystem->textFieldInputId);
            if (tf) {
                Text* txt = GetText(tf->textId);
                if (txt)
                    pg::SetText(txt->id, std::format("{}{}", txt->text->text ? txt->text->text : "", text));
            }
        }
            continue;
        case SDL_EVENT_KEY_DOWN: {
            if (e.key.key == SDLK_BACKSPACE && cSystem->textFieldInputId) {

                TextField* tf = GetTextField(cSystem->textFieldInputId);
                if (tf) {
                    Text* txt = GetText(tf->textId);
                    if (txt && txt->text->text) {
                        std::string text = txt->text->text;
                        if (!text.empty()) {
                            text.pop_back();
                            pg::SetText(txt->id, text);
                        }
                    }
                }
            }
        }
        default:
            continue;
        }
    }
}

bool Init() {
    cSystem = new System();

    buttons        = &cSystem->buttons;
    scenes         = &cSystem->scenes;
    textFields     = &cSystem->textFields;
    textureAtlases = &cSystem->textureAtlases;

    pg::LinkPluginFunction(InterfaceLoop, &cSystem->linkToPlugin);

    buttonFunctions.insert({"TextFieldDown", TextFieldDown});

    pg::Log(0, "Successfully intitialized interface");
    return 1;
}
bool Quit() {
    if (!cSystem) {
        pg::Log(1, "Cannot quit, cSystem is nullptr");
        return 0;
    }

    delete cSystem;

    pg::Log(0, "Successfully exited interface");
    return 1;
}

id_t RequestTransferBuffer() {
    id_t tbId = pg::GetLowestKey(pg::transferBuffers, pg::START_SYSTEM_RESOURCES);
    if (!pg::CreateTransferBuffer(tbId, {.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = pg::DEFAULT_DYNAMIC_SIZE})) {
        return 0;
    }

    if (!pg::SetTransferBufferAttributes(tbId, {{pg::MANAGEMENT_TYPE, pg::MANAGEMENT_DYNAMIC}})) {
        return 0;
    }
    cSystem->transferBufferIds.push_back(tbId);

    pg::Log(0, "Successfully requested transferBuffer");
    return tbId;
}

} // namespace pg::interface
