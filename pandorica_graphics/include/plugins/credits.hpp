#pragma once

#include "../pg.hpp"

/** @file */
namespace pg {
namespace credits {

/** @internal Refresh the screen. */
inline void Refresh() {
    id_t camId = GetLowestKey(cameras, settings::START_SYSTEM_RESOURCES) - 1;
    SetCameraAttributes(camId, {{ASPECT_RATIO, static_cast<float>(window->width) / window->height}});
    pg::Render(camId);
    if (window->timeMS > 5000) {
        cContext->running = 0;
    }
};

/**
 * @brief Play the Pandorica Graphics credits.
 *
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @ingroup pg_plugin
 */
inline bool PlayCredits() {
    auto bg               = settings::CLEAR_COLOR;
    settings::CLEAR_COLOR = {0.12, 0.12, 0.16, 1.0f};

    id_t texTBId = GetLowestKey(transferBuffers, settings::START_SYSTEM_RESOURCES);
    if (!CreateTransferBuffer(texTBId, {.size = settings::DEFAULT_TEXTURETRANSFERBUFFER_SIZE}))
        return 0;
    id_t oTBId = GetLowestKey(transferBuffers, settings::START_SYSTEM_RESOURCES);
    if (!CreateTransferBuffer(oTBId, {.size = settings::DEFAULT_DYNAMIC_SIZE}))
        return 0;

    id_t tLogoId = GetLowestKey(textures, settings::START_SYSTEM_RESOURCES);
    if (!CreateTexture(tLogoId, texTBId, "pandorica_graphics/assets/logo.png"))
        return 0;

    id_t tTextBoxId = GetLowestKey(textures, settings::START_SYSTEM_RESOURCES);
    if (!CreateTexture(tTextBoxId, texTBId, "pandorica_graphics/assets/textbox.png"))
        return 0;
    UploadTextures(texTBId, oTBId);

    id_t fontId = GetLowestKey(fonts, settings::START_SYSTEM_RESOURCES);
    if (!CreateFont(fontId, 132, "pandorica_graphics/assets/pixel.ttf"))
        return 0;

    id_t textId = GetLowestKey(texts, settings::START_SYSTEM_RESOURCES);
    if (!CreateText(textId, fontId, oTBId))
        return 0;

    id_t oLogoId = GetLowestKey(objects, settings::START_SYSTEM_RESOURCES);
    CreateObject(oLogoId, tLogoId);

    id_t oTextBoxId = GetLowestKey(objects, settings::START_SYSTEM_RESOURCES);
    CreateObject(oTextBoxId, tTextBoxId);

    id_t camId = GetLowestKey(cameras, settings::START_SYSTEM_RESOURCES);
    CreateCamera(camId);

    id_t wsId = GetLowestKey(workspaces, settings::START_SYSTEM_RESOURCES);
    CreateWorkspace(wsId);

    id_t bId = GetLowestKey(buffers, settings::START_SYSTEM_RESOURCES);
    CreateBuffer(bId, {.usage = SDL_GPU_BUFFERUSAGE_VERTEX, .size = settings::DEFAULT_DYNAMIC_SIZE});

    SetTransferBufferAttributes(oTBId, {{WORKSPACE_ID, wsId}});
    SetCameraAttributes(camId, {{ADD_WORKSPACE_ID, wsId}, {TARGET_X, 0}, {TARGET_Y, 0}, {TARGET_Z, 0}, {POSITION_Z, -1}, {POSITION_X, 0}, {DDD, 0}});

    SetObjectAttributes(oLogoId, {{WIDTH, 1.f}, {HEIGHT, 1.f}, {POSITION_Y, -0.8f}, {POSITION_X, 0}, {ORIGIN_X, 0.5f}, {RED, 255}, {BLUE, 255}, {GREEN, 255}});
    SetTextAttributes(textId, {{ORIGIN_X, 0.5f}, {ORIGIN_Y, 0.5f}, {POSITION_Y, 0.6f}, {RED, 0xEE}, {BLUE, 0xEE}, {GREEN, 0xEE}});

    SetText(textId, "Made with Pandorica Graphics");

    SetObjectAttributes(oTextBoxId, {{ORIGIN_X, 0.5f}, {ORIGIN_Y, 0.5f}, {POSITION_Y, 0.6f}, {WIDTH, 1.8f}, {HEIGHT, 0.25f}});

    UploadData({oTBId});

    auto tb = GetTransferBuffer(oTBId);
    auto ws = GetWorkspace(wsId);

    pg::Render(camId);
    window->frameCount = 0;
    pg::StartLoop(Refresh, 120);

    settings::CLEAR_COLOR = bg;

    DeleteTransferBuffer(texTBId);
    DeleteTransferBuffer(oTBId);
    DeleteTexture(tLogoId);
    DeleteTexture(tTextBoxId);
    DeleteFont(fontId);
    DeleteText(textId);
    DeleteObject(oLogoId);
    DeleteObject(oTextBoxId);
    DeleteCamera(camId);
    DeleteWorkspace(wsId);
    DeleteBuffer(bId);

    return 1;
}

} // namespace credits
} // namespace pg
