#pragma once

#include "generic.hpp"

namespace pg {
namespace interface {

/** @file */
struct TextField {
    const id_t textFieldId = 0;

    id_t sceneId = 0;

    bool enabled = 1;

    id_t textId   = 0;
    id_t buttonId = 0;

    bool writable       = 1;
    bool isBeingWritten = 0;

    SDL_PropertiesID textFieldInputProperties = 0;

    TextField(const id_t textFieldId);
    ~TextField();
};
extern pg::idmap<TextField>* textFields;

bool       CreateTextField(id_t textFieldId, id_t textId);
bool       DeleteTextField(id_t textFieldId);
bool       SetTextFieldAttributes(id_t textFieldId, const std::vector<std::pair<interface::Attribute, pg::GenericValue32>>& attributes);
bool       SetTextFieldText(id_t textFieldId, std::string_view text);
TextField* GetTextField(id_t textFieldId);

} // namespace interface
} // namespace pg
