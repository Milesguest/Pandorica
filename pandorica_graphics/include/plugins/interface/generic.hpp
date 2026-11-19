#pragma once

#include "../../pg.hpp"

/** @file */
namespace pg {
namespace interface {

enum FEAttribute {
    KEY_DOWN_FUNC = pg::Attribute::OBJECT_ID + 1,
    KEY_UP_FUNC,
    HOVER_DOWN_FUNC,
    HOVER_UP_FUNC,
    SCENE_ID,
    TEXT_ID,
    WRITABLE,
    BUTTON_ID
};
using Attribute = uint32_t;

} // namespace interface
} // namespace pg
