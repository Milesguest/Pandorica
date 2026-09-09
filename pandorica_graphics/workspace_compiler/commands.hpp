#pragma once

#include "enums.hpp"
#include <pandorica_graphics/pg.hpp>

namespace pg {
namespace ws {

#define WS_COMMANDS                                           \
    Y(o, "o", pg::ws::CreateObject, -1)                       \
    X(tex, "tex", pg::ws::CreateTexture, ws::Attr::TEXID)     \
    Y(cam, "cam", pg::ws::CreateCamera, -1)                   \
    X(ws, "ws", pg::ws::CreateWorkspace, ws::Attr::WSID)      \
    X(tb, "tb", pg::ws::CreateTransferBuffer, ws::Attr::TBID) \
    Y(ib, "ib", pg::ws::CreateBuffer, -1)                     \
    Y(ad, "ad", pg::ws::CreateAudioData, -1)                  \
    Y(txt, "txt", pg::ws::CreateText, -1)

#define X(enum, string, function, attribute) std::pair<int, id_t>{static_cast<int>(attribute), 0},
#define Y(enum, string, function, attribute)
inline auto lastCommands = std::unordered_map{WS_COMMANDS};
#undef X
#undef Y

inline bool CreateObject(pg::vector<std::pair<int, GenericValue32>>& attributes, pg::vector<std::pair<int, std::string>>& attributesString) {
    bool error = 1;

    std::vector<std::pair<pg::Attribute, GenericValue32>> pgAttributes{};

    id_t oId = GetLowestKey(objects, pg::settings::START_SYSTEM_RESOURCES);
    id_t tId = 0;

    for (auto it = attributes.begin(); it != attributes.end();) {
        auto attribute = *it;

        if (attribute.first == static_cast<int>(ws::Attr::ID)) {
            oId = attribute.second.asUint();
            it  = attributes.erase(it);
            continue;
        }
        if (attribute.first == static_cast<int>(ws::Attr::TEXID)) {
            tId = attribute.second.asUint();
            it  = attributes.erase(it);
            continue;
        }

        int pgAttr = GetPGByAttribute(static_cast<ws::Attr>(attribute.first));
        if (pgAttr < 0) {
            it = attributes.erase(it);
            continue;
        }

        pgAttributes.push_back({static_cast<pg::Attribute>(pgAttr), attribute.second});

        ++it;
    }

    if (!objects->contains(oId)) {
        if (!tId)
            tId = lastCommands.at(static_cast<int>(ws::Attr::TEXID));
        error &= pg::CreateObject(oId, tId);
    } else {
        if (tId)
            pgAttributes.push_back({pg::TEXTURE_ID, tId});
    }
    error &= pg::SetObjectAttributes(oId, pgAttributes);

    return error;
}

inline bool CreateTexture(pg::vector<std::pair<int, GenericValue32>>& attributes, pg::vector<std::pair<int, std::string>>& attributesString) {
    bool error = 1;

    std::vector<std::pair<pg::Attribute, GenericValue32>> pgAttributes{};

    id_t tId  = GetLowestKey(textures, pg::settings::START_SYSTEM_RESOURCES);
    id_t tbId = 0;

    for (auto it = attributes.begin(); it != attributes.end();) {
        auto attribute = *it;

        if (attribute.first == static_cast<int>(ws::Attr::ID)) {
            tId = attribute.second.asUint();
            it  = attributes.erase(it);
            continue;
        }
        if (attribute.first == static_cast<int>(ws::Attr::TBID)) {
            tbId = attribute.second.asUint();
            it   = attributes.erase(it);
            continue;
        }

        int pgAttr = GetPGByAttribute(static_cast<ws::Attr>(attribute.first));
        if (pgAttr < 0) {
            it = attributes.erase(it);
            continue;
        }

        pgAttributes.push_back({static_cast<pg::Attribute>(pgAttr), attribute.second});

        ++it;
    }

    std::string_view path = "";

    for (std::pair<int, std::string_view> attribute : attributesString) {
        if (attribute.first == static_cast<int>(ws::Attr::PATH)) {
            path = attribute.second;
            break;
        }
    }

    if (path != "") {
        if (!tbId)
            tbId = lastCommands.at(static_cast<int>(ws::Attr::TBID));
        error &= pg::CreateTexture(tId, tbId, path);
    } else {
        if (tbId)
            pgAttributes.push_back({pg::TRANSFERBUFFER_ID, tbId});
    }
    error &= pg::SetTextureAttributes(tId, pgAttributes);

    lastCommands.at(static_cast<int>(ws::Attr::TEXID)) = tId;

    return error;
}

inline bool CreateCamera(pg::vector<std::pair<int, GenericValue32>>& attributes, pg::vector<std::pair<int, std::string>>& attributesString) {
    bool error = 1;

    std::vector<std::pair<pg::Attribute, GenericValue32>> pgAttributes{};

    id_t camId = GetLowestKey(cameras, pg::settings::START_SYSTEM_RESOURCES);

    for (auto it = attributes.begin(); it != attributes.end();) {
        auto attribute = *it;

        if (attribute.first == static_cast<int>(ws::Attr::ID)) {
            camId = attribute.second.asUint();
            it    = attributes.erase(it);
            continue;
        }

        int pgAttr = GetPGByAttribute(static_cast<ws::Attr>(attribute.first));
        if (pgAttr < 0) {
            it = attributes.erase(it);
            continue;
        }

        pgAttributes.push_back({static_cast<pg::Attribute>(pgAttr), attribute.second});

        ++it;
    }

    if (!cameras->contains(camId))
        error &= pg::CreateCamera(camId);

    error &= pg::SetCameraAttributes(camId, pgAttributes);

    return error;
}

inline bool CreateWorkspace(pg::vector<std::pair<int, GenericValue32>>& attributes, pg::vector<std::pair<int, std::string>>& attributesString) {
    bool error = 1;

    std::vector<std::pair<pg::Attribute, GenericValue32>> pgAttributes{};

    id_t wsId = GetLowestKey(workspaces, pg::settings::START_SYSTEM_RESOURCES);

    for (auto it = attributes.begin(); it != attributes.end();) {
        auto attribute = *it;

        if (attribute.first == static_cast<int>(ws::Attr::ID)) {
            wsId = attribute.second.asUint();
            it   = attributes.erase(it);
            continue;
        }

        int pgAttr = GetPGByAttribute(static_cast<ws::Attr>(attribute.first));
        if (pgAttr < 0) {
            it = attributes.erase(it);
            continue;
        }

        pgAttributes.push_back({static_cast<pg::Attribute>(pgAttr), attribute.second});

        ++it;
    }

    error &= pg::CreateWorkspace(wsId);

    lastCommands.at(static_cast<int>(ws::Attr::WSID)) = wsId;

    return error;
}

inline bool CreateTransferBuffer(pg::vector<std::pair<int, GenericValue32>>& attributes, pg::vector<std::pair<int, std::string>>& attributesString) {
    bool error = 1;

    std::vector<std::pair<pg::Attribute, GenericValue32>> pgAttributes{};

    id_t   tbId = GetLowestKey(transferBuffers, pg::settings::START_SYSTEM_RESOURCES);
    id_t   wsId = 0;
    size_t size = settings::DEFAULT_DYNAMIC_SIZE;
    for (auto it = attributes.begin(); it != attributes.end();) {
        auto attribute = *it;

        if (attribute.first == static_cast<int>(ws::Attr::ID)) {
            tbId = attribute.second.asUint();
            it   = attributes.erase(it);
            continue;
        }

        if (attribute.first == static_cast<int>(ws::Attr::WSID)) {
            wsId = attribute.second.asUint();
            it   = attributes.erase(it);
            continue;
        }

        if (attribute.first == static_cast<int>(ws::Attr::BSIZE)) {
            size = attribute.second.asUint();
            it   = attributes.erase(it);
            continue;
        }

        int pgAttr = GetPGByAttribute(static_cast<ws::Attr>(attribute.first));
        if (pgAttr < 0) {
            it = attributes.erase(it);
            continue;
        }

        pgAttributes.push_back({static_cast<pg::Attribute>(pgAttr), attribute.second});

        ++it;
    }

    if (!transferBuffers->contains(tbId)) {
        if (!wsId)
            wsId = lastCommands.at(static_cast<int>(ws::Attr::WSID));
        error &= pg::CreateTransferBuffer(tbId, {.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = size});
    }
    if (wsId)
        pgAttributes.push_back({static_cast<pg::Attribute>(GetPGByAttribute(ws::Attr::WSID)), wsId});

    error &= pg::SetTransferBufferAttributes(tbId, pgAttributes);

    lastCommands.at(static_cast<int>(ws::Attr::TBID)) = tbId;

    return error;
}

inline bool CreateBuffer(pg::vector<std::pair<int, GenericValue32>>& attributes, pg::vector<std::pair<int, std::string>>& attributesString) {
    bool error = 1;

    std::vector<std::pair<pg::Attribute, GenericValue32>> pgAttributes{};

    id_t   ibId = GetLowestKey(buffers, pg::settings::START_SYSTEM_RESOURCES);
    size_t size = settings::DEFAULT_DYNAMIC_SIZE;

    for (auto it = attributes.begin(); it != attributes.end();) {
        auto attribute = *it;

        if (attribute.first == static_cast<int>(ws::Attr::ID)) {
            ibId = attribute.second.asUint();
            it   = attributes.erase(it);
            continue;
        }

        if (attribute.first == static_cast<int>(ws::Attr::BSIZE)) {
            size = attribute.second.asUint();
            it   = attributes.erase(it);
            continue;
        }

        int pgAttr = GetPGByAttribute(static_cast<ws::Attr>(attribute.first));
        if (pgAttr < 0) {
            it = attributes.erase(it);
            continue;
        }

        pgAttributes.push_back({static_cast<pg::Attribute>(pgAttr), attribute.second});

        ++it;
    }

    if (!buffers->contains(ibId))
        error &= pg::CreateBuffer(ibId, {.usage = SDL_GPU_BUFFERUSAGE_VERTEX, .size = size});

    error &= pg::SetBufferAttributes(ibId, pgAttributes);

    return error;
}

inline bool CreateAudioData(pg::vector<std::pair<int, GenericValue32>>& attributes, pg::vector<std::pair<int, std::string>>& attributesString) {
    bool error = 1;

    std::vector<std::pair<pg::Attribute, GenericValue32>> pgAttributes{};

    id_t adId = GetLowestKey(audioDatas, pg::settings::START_SYSTEM_RESOURCES);
    id_t wsId = 0;

    for (auto it = attributes.begin(); it != attributes.end();) {
        auto attribute = *it;

        if (attribute.first == static_cast<int>(ws::Attr::ID)) {
            adId = attribute.second.asUint();
            it   = attributes.erase(it);
            continue;
        }
        if (attribute.first == static_cast<int>(ws::Attr::WSID)) {
            wsId = attribute.second.asUint();
            it   = attributes.erase(it);
            continue;
        }

        int pgAttr = GetPGByAttribute(static_cast<ws::Attr>(attribute.first));
        if (pgAttr < 0) {
            it = attributes.erase(it);
            continue;
        }

        pgAttributes.push_back({static_cast<pg::Attribute>(pgAttr), attribute.second});

        ++it;
    }

    std::string_view path = "";

    for (std::pair<int, std::string_view> attribute : attributesString) {
        if (attribute.first == static_cast<int>(ws::Attr::PATH)) {
            path = attribute.second;
            break;
        }
    }

    if (path != "") {
        if (!wsId)
            wsId = lastCommands.at(static_cast<int>(ws::Attr::WSID));
        error &= pg::CreateAudioData(adId, path);
    }
    if (wsId)
        pgAttributes.push_back({pg::WORKSPACE_ID, wsId});
    error &= pg::SetAudioDataAttributes(adId, pgAttributes);

    return error;
}

inline bool CreateText(pg::vector<std::pair<int, GenericValue32>>& attributes, pg::vector<std::pair<int, std::string>>& attributesString) {
    bool error = 1;

    std::vector<std::pair<pg::Attribute, GenericValue32>> pgAttributes{};

    id_t txtId  = GetLowestKey(texts, pg::settings::START_SYSTEM_RESOURCES);
    id_t tbId   = 0;
    id_t fontId = 0;

    for (auto it = attributes.begin(); it != attributes.end();) {
        auto attribute = *it;

        if (attribute.first == static_cast<int>(ws::Attr::ID)) {
            txtId = attribute.second.asUint();
            it    = attributes.erase(it);
            continue;
        }
        if (attribute.first == static_cast<int>(ws::Attr::TBID)) {
            tbId = attribute.second.asUint();
            it   = attributes.erase(it);
            continue;
        }
        if (attribute.first == static_cast<int>(ws::Attr::FONTID)) {
            fontId = attribute.second.asUint();
            it     = attributes.erase(it);
            continue;
        }

        int pgAttr = GetPGByAttribute(static_cast<ws::Attr>(attribute.first));
        if (pgAttr < 0) {
            it = attributes.erase(it);
            continue;
        }

        pgAttributes.push_back({static_cast<pg::Attribute>(pgAttr), attribute.second});

        ++it;
    }

    std::string_view text = "";

    for (std::pair<int, std::string_view> attribute : attributesString) {
        if (attribute.first == static_cast<int>(ws::Attr::TEXT)) {
            text = attribute.second;
            break;
        }
    }

    if (!texts->contains(txtId)) {
        if (!tbId)
            tbId = lastCommands.at(static_cast<int>(ws::Attr::TBID));
        error &= pg::CreateText(txtId, fontId, tbId);
    } else {
        if (tbId)
            pgAttributes.push_back({pg::TRANSFERBUFFER_ID, tbId});
        if (fontId)
            pgAttributes.push_back({pg::TEXTURE_ID, fontId});
    }
    error &= pg::SetTextAttributes(txtId, pgAttributes);

    if (text != "") {
        error &= SetText(txtId, text);
    }

    return error;
}

#define X(enum, string, function, attribute) enum,
#define Y(enum, string, function, attribute) enum,
enum class Command : uint8_t {
    WS_COMMANDS
};
#undef X
#undef Y

#define X(enum, string, function, attribute) std::pair<std::string_view, Command>{string, Command::enum},
#define Y(enum, string, function, attribute) std::pair<std::string_view, Command>{string, Command::enum},
inline constexpr auto commandStrings = std::array{WS_COMMANDS};
#undef X
#undef Y

inline constexpr std::optional<pg::ws::Command> GetCommandByName(std::string_view name) {
    for (const auto& pair : pg::ws::commandStrings) {
        if (pair.first == name) {
            return pair.second;
        }
    }
    return std::nullopt;
}

#define X(enum, string, function, attribute) \
    case Command::enum:                      \
        return function;
#define Y(enum, string, function, attribute) \
    case Command::enum:                      \
        return function;
using FuncPtrReturn = bool (*)(pg::vector<std::pair<int, GenericValue32>>&,
                               pg::vector<std::pair<int, std::string>>&);
inline constexpr FuncPtrReturn GetFunctionByCommand(Command command) {
    switch (command) {
        WS_COMMANDS
    }
    return nullptr;
}
#undef X
#undef Y

#undef WS_COMMANDS

} // namespace ws
} // namespace pg
