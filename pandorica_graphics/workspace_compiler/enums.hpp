#pragma once

#include <map>
#include <string>
#include <vector>

#include "../include/pg_generic.hpp"

/** @file */
namespace pg {
namespace ws {

inline constexpr std::array<std::pair<uint8_t, std::string_view>, 8> commands = {{
    {1, "o"},
    {2, "tex"},
    {3, "cam"},
    {4, "ws"},
    {5, "tb"},
    {6, "db"},
    {7, "ad"},
    {8, "txt"},
}};
#define WC_ATTRIBUTES                            \
    X(POSX, "POSX", pg::POSITION_X)              \
    X(POSY, "POSY", pg::POSITION_Y)              \
    X(POSZ, "POSZ", pg::POSITION_Z)              \
    X(WIDTH, "WIDTH", pg::WIDTH)                 \
    X(HEIGHT, "HEIGHT", pg::HEIGHT)              \
    X(ROTX, "ROTX", pg::ROTATION_X)              \
    X(ROTY, "ROTY", pg::ROTATION_Y)              \
    X(ROTZ, "ROTZ", pg::ROTATION_Z)              \
    X(CUTX, "CUTX", pg::CUT_X)                   \
    X(CUTY, "CUTY", pg::CUT_Y)                   \
    X(CUTW, "CUTW", pg::CUT_WIDTH)               \
    X(CUTH, "CUTH", pg::CUT_HEIGHT)              \
    X(ORIX, "ORIX", pg::ORIGIN_X)                \
    X(ORIY, "ORIY", pg::ORIGIN_Y)                \
    X(ORIZ, "ORIZ", pg::ORIGIN_Z)                \
    X(R, "R", pg::RED)                           \
    X(G, "G", pg::GREEN)                         \
    X(B, "B", pg::BLUE)                          \
    X(A, "A", pg::ALPHA)                         \
    X(TEXID, "TEXID", pg::TEXTURE_ID)            \
    X(E, "E", pg::ENABLED)                       \
    X(TBID, "TBID", pg::TRANSFERBUFFER_ID)       \
    X(TBOFF, "TBOFF", pg::TRANSFERBUFFER_OFFSET) \
    X(WSID, "WSID", pg::WORKSPACE_ID)            \
    X(SAMID, "SAMID", pg::SAMPLER_ID)            \
    X(IBID, "IBID", pg::INSTANCEBUFFER_ID)       \
    X(IBOFF, "IBOFF", pg::INSTANCEBUFFER_OFFSET) \
    X(MANAGE, "MANAGE", pg::MANAGEMENT_TYPE)     \
    X(TARX, "TARX", pg::TARGET_X)                \
    X(TARY, "TARY", pg::TARGET_Y)                \
    X(TARZ, "TARZ", pg::TARGET_Z)                \
    X(FOVY, "FOVY", pg::FOV_Y)                   \
    X(AR, "AR", pg::ASPECT_RATIO)                \
    X(DDD, "DDD", pg::DDD)                       \
    X(ADDWSID, "ADDWSID", pg::ADD_WORKSPACE_ID)  \
    X(RMWSID, "RMWSID", pg::REMOVE_WORKSPACE_ID) \
    X(ID, "ID", -1)                              \
    X(PATH, "PATH", -1)                          \
    X(BSIZE, "BSIZE", -1)                        \
    X(OBJID, "OBJID", -1)                        \
    Y(WIDTH, "SIZE", -1)                         \
    Y(CUTX, "CUTPOS", -1)                        \
    Y(CUTW, "CUTSIZE", -1)                       \
    Y(POSX, "POS", -1)                           \
    Y(ROTX, "ROT", -1)                           \
    Y(TARX, "TAR", -1)                           \
    Y(ORIX, "ORIPOS", -1)                        \
    Y(R, "COLOR", -1)

#define X(enum, string, pg) enum,
#define Y(enum, string, pg)
enum class Attr : uint8_t {
    WC_ATTRIBUTES
};

#undef X
#undef Y

#define X(enum, string, pg) {string, Attr::enum},
#define Y(enum, string, pg) {string, Attr::enum},
inline constexpr std::array<std::pair<std::string_view, Attr>, 48> attrStrings = {{WC_ATTRIBUTES}};
#undef X
#undef Y

#define X(enum, string, pg) \
    case Attr::enum:        \
        return pg;
#define Y(enum, string, pg)
constexpr int GetPG(Attr attr) {
    switch (attr) {
        WC_ATTRIBUTES
    }
}
#undef X
#undef Y

inline std::optional<pg::ws::Attr> GetAttributeByName(std::string_view name) {
    for (const auto& pair : pg::ws::attrStrings) {
        if (pair.first == name) {
            return pair.second;
        }
    }
    return std::nullopt;
}

inline std::string_view GetAttributeName(pg::ws::Attr attr) {
    for (const auto& pair : pg::ws::attrStrings) {
        if (pair.second == attr) {
            return pair.first;
        }
    }
    return "";
}

} // namespace ws
} // namespace pg
