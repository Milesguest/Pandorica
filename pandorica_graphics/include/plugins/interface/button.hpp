#pragma once

#include "generic.hpp"

/** @file */
namespace pg {
namespace interface {

/**
 *
 */
struct Button {
    const id_t buttonId = 0;

    bool enabled = 1;

    pg::vec2f position = {0.f, 0.f};
    pg::vec2f size     = {1.f, 1.f};

    id_t sceneId  = 0;
    id_t objectId = 0;

    bool isClicked = false;
    bool isHovered = false;

    std::string keyDownFuncPtr   = "";
    std::string keyUpFuncPtr     = "";
    std::string hoverDownFuncPtr = "";
    std::string hoverUpFuncPtr   = "";

    Button(id_t buttonId);
    ~Button();
};
extern pg::idmap<Button>*                          buttons;
extern std::unordered_map<std::string, void (*)()> buttonFunctions;

bool    CreateButton(id_t buttonId);
bool    DeleteButton(id_t buttonId);
bool    SetButtonAttributes(id_t buttonId, const std::vector<std::pair<interface::Attribute, std::variant<pg::GenericValue32, const char*>>>& attributes);
Button* GetButton(id_t buttonId);

} // namespace interface
} // namespace pg
