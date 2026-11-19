#pragma once

#include "button.hpp"
#include "scene.hpp"
#include "textField.hpp"
#include "textureAtlas.hpp"

/** @file */
namespace pg {
namespace interface {

struct System {
    pg::idmap<Button>       buttons{};
    pg::idmap<TextureAtlas> textureAtlases{};
    pg::idmap<TextField>    textFields{};
    pg::idmap<Scene>        scenes{};

    std::vector<id_t> transferBufferIds{};

    pg::vec2f mousePosition{};

    id_t hoveredButtonId  = 0;
    id_t clickedButtonId  = 0;
    id_t textFieldInputId = 0;

    void (*linkToPlugin)();
};
extern System* cSystem;

bool Init();
bool Quit();
bool CreatePGDevices(const std::string_view title, const size_t width, size_t height, const SDL_WindowFlags flags, const bool debug);

id_t RequestTransferBuffer();

} // namespace interface
} // namespace pg
