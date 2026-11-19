#include "../../../include/plugins/interface/textField.hpp"
#include "../../../include/plugins/interface/system.hpp"

namespace pg::interface {

pg::idmap<TextField>* textFields;

TextField::TextField(id_t textFieldId) : textFieldId(textFieldId) {
    (*textFields)[textFieldId] = this;
}

TextField::~TextField() {
    textFields->erase(textFieldId);
}

bool CreateTextField(id_t textFieldId, id_t textId) {
    if (!textFieldId) {
        pg::Log(1, "TextField id is not allowed to be 0, cannot create the textField");
        return 0;
    }

    if (textFields->contains(textFieldId)) {
        pg::Log(1, std::format("TextField at id {} already exists, deleting it.", textFieldId));
        DeleteTextField(textFieldId);
    }

    if (!pg::texts->contains(textId)) {
        pg::Log(1, std::format("Text at id {} does not exist, cannot create textField", textId));
        return 0;
    }

    TextField* textField = new TextField(textFieldId);

    textField->textId = textId;

    pg::Log(0, std::format("Successfully created TextField at id {}", textFieldId));
    return 1;
}

bool DeleteTextField(id_t textFieldId) {
    if (!textFields->contains(textFieldId)) {
        pg::Log(0, std::format("TextField at id {} does not exist, cannot delete it", textFieldId));
        return 0;
    }

    bool error = 1;

    TextField* textField = textFields->at(textFieldId);

    error &= pg::DeleteText(textField->textId);
    delete textField;

    pg::Log(0, std::format("Successfully deleted TextField at id {}", textFieldId));
    return error;
}

bool SetTextFieldAttributes(id_t textFieldId, const std::vector<std::pair<interface::Attribute, pg::GenericValue32>>& attributes) {
    if (!textFields->contains(textFieldId)) {
        pg::Log(1, std::format("TextField at id {} does not exist. Cannot set its attributes", textFieldId));
        return 0;
    }

    TextField* textField = textFields->at(textFieldId);
    bool       error     = 1;

    for (auto i : attributes)
        switch (i.first) {
        case pg::ENABLED:
            textField->enabled = i.second.asBool();
            break;
        case SCENE_ID:
            if (!scenes->contains(i.second.asUint())) {
                pg::Log(1, std::format("Scene at id {} does not exist. Cannot set it as attribute for textField", i.second.asUint()));
                error = 0;
                break;
            }
            textField->sceneId = i.second.asUint();
            scenes->at(i.second.asUint())->textFields.push_back(textField);
            break;
        case TEXT_ID:
            if (!pg::texts->contains(i.second.asUint())) {
                pg::Log(1, std::format("Text at id {} does not exist. Cannot set it as attribute for textField", i.second.asUint()));
                error = 0;
                break;
            }
            textField->textId = i.second.asUint();
            break;
        case WRITABLE:
            textField->writable = i.second.asBool();
            break;
        case BUTTON_ID:
            if (!buttons->contains(i.second.asUint())) {
                pg::Log(1, std::format("Button at id {} does not exist. Cannot set it as attribute.", i.second.asUint()));
                error = 0;
                break;
            }
            textField->buttonId = i.second.asUint();
            SetButtonAttributes(i.second.asUint(), {{KEY_DOWN_FUNC, "TextFieldDown"}});
            break;
        default:
            pg::Log(1, "Attribute is unavailable for type textField.");
            error = 0;
            break;
        }

    pg::Log(0, std::format("Succesfully set attributes for textField at id {}", textFieldId));
    return error;
}

bool SetTextFieldText(id_t textFieldId, std::string_view text) {
    if (!textFields->contains(textFieldId)) {
        pg::Log(0, std::format("TextField at id {} does not exist, cannot set its text", textFieldId));
        return 0;
    }

    TextField* textField = textFields->at(textFieldId);

    bool error = 1;

    error &= pg::SetText(textField->textId, text.data());

    return error;
}

TextField* GetTextField(id_t textFieldId) {
    if (!textFields->contains(textFieldId)) {
        pg::Log(1, std::format("TextField at id {} does not exist, cannot get it", textFieldId));
        return nullptr;
    }

    pg::Log(0, std::format("Successfully got textField at id {}", textFieldId));
    return textFields->at(textFieldId);
}

TTF_Text* GetTextFieldTTFData(id_t textFieldId) {
    if (!textFields->contains(textFieldId)) {
        pg::Log(1, std::format("TextField at id {} does not exist, cannot get its TTF data", textFieldId));
        return nullptr;
    }

    pg::Text* text = pg::GetText(textFields->at(textFieldId)->textId);
    if (!text) {
        pg::Log(1, std::format("Text at id {} does not exist, cannot get its TTF data", text->id));
        return nullptr;
    }

    pg::Log(0, std::format("Successfully got textFieldTTF data from id {}", textFieldId));
    return text->text;
}

} // namespace pg::interface
