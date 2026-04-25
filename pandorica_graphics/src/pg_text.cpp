#include "../include/pg_text.hpp"
#include "../include/pg_main.hpp"

namespace pg {

idmap<Text>* texts;

Text::Text(id_t id, id_t fontId, id_t transferBufferId) : EngineResource(id) {
    (*texts)[id] = this;

    this->fontId = fontId;
    fonts->at(fontId)->texts.push_back(this);
    this->transferBufferId = transferBufferId;
    transferBuffers->at(transferBufferId)->texts.push_back(this);

    this->instance = std::make_shared<Instance>();
}
Text::~Text() {
    if (text) {
        TTF_DestroyText(text);
    }
    texts->erase(id);

    transferBuffers->at(transferBufferId)->texts.remove(this);
    fonts->at(fontId)->texts.remove(this);
}

inline void CreateLetters(Text* text) {
    if (text->instance.use_count() < 2) {
        text->attachedObjectId = 0;
    }
    TTF_GPUAtlasDrawSequence* drawData = TTF_GetGPUTextDrawData(text->text);

    text->letterAtlas = drawData->atlas_texture;

    std::vector<SDL_FPoint> coordinates(drawData->xy, drawData->xy + drawData->num_vertices);

    text->letters.clear();

    for (int i = 0; i < drawData->num_vertices / 4; ++i) {
        text->letters.push_back(Instance());
        Instance& letter = text->letters.back();

        letter.cutPos  = {drawData->uv[i * 4].x, drawData->uv[i * 4].y};
        letter.cutSize = {drawData->uv[i * 4 + 2].x, drawData->uv[i * 4 + 2].y};
        letter.cutSize -= letter.cutPos;

        letter.position = {coordinates[i * 4].x / 1024, -coordinates[i * 4].y / 1024, -0.001f};
        letter.size     = {coordinates[i * 4 + 2].x / 1024, -coordinates[i * 4 + 2].y / 1024};
        letter.size -= letter.position.head<2>();
        letter.origin = -letter.position;
        letter.origin.head<2>().array() /= letter.size.array();
    }

    vec4f bounds{MAXFLOAT, MAXFLOAT, -MAXFLOAT, -MAXFLOAT};
    for (Instance& letter : text->letters) {
        if (letter.position.x() < bounds.x())
            bounds.x() = letter.position.x();
        if (letter.position.x() + letter.size.x() > bounds.z())
            bounds.z() = letter.position.x() + letter.size.x();
        if (letter.position.y() < bounds.y())
            bounds.y() = letter.position.y();
        if (letter.position.y() + letter.size.y() > bounds.w())
            bounds.w() = letter.position.y() + letter.size.y();
    }

    bounds.z() -= bounds.x();
    bounds.x() = 0.0f;

    bounds.w() -= bounds.y();
    bounds.y() = 0.0f;

    vec2f lettersOrigin{
        std::lerp(bounds.x(), bounds.z(), text->instance->origin.x()),
        std::lerp(bounds.y(), bounds.w(), text->instance->origin.y()),
    };

    for (Instance& letter : text->letters) {
        letter.origin.head<2>().array() += lettersOrigin.array() / letter.size.array();
    }
}

bool CreateText(const id_t textId, const id_t fontId, const id_t transferBufferId) {
    if (!textId) {
        Log(1, "TextId is not allowed to be 0");
        return 0;
    }

    if (texts->contains(textId)) {
        Log(1, std::format("Text at id {} already exists, deleting it", textId));
        delete texts->at(textId);
    }

    if (!fonts->contains(fontId)) {
        Log(1, std::format("Font at id {} does not exist, cannot create text", fontId));
        return 0;
    }

    if (!transferBuffers->contains(transferBufferId)) {
        Log(1, std::format("TransferBuffer at id {} does not exist, cannot create text", transferBufferId));
        return 0;
    }

    Font* font = fonts->at(fontId);

    Text* text = new Text(textId, fontId, transferBufferId);

    text->text = TTF_CreateText(textDevice->textEngine, font->font, "NULL", 0);

    Log(0, std::format("Successfully created text at id {}", textId));
    return 1;
}

bool DeleteText(const id_t textId) {
    if (!texts->contains(textId)) {
        Log(1, std::format("Text at id {} does not exist, exiting delete function", textId));
        return 0;
    }

    Text* text = texts->at(textId);

    if (text->attachedObjectId >= settings::START_SYSTEM_RESOURCES)
        DeleteObject(text->attachedObjectId);

    delete text;

    Log(0, std::format("Successfully deleted text at id {}", textId));
    return 1;
}

bool SetText(const id_t textId, const std::string& textData) {
    if (!texts->contains(textId)) {
        Log(1, std::format("Text at id {} does not exist, cannot change text", textId));
        return 0;
    }

    Text* text = texts->at(textId);

    if (!TTF_SetTextString(text->text, textData.c_str(), 0)) {
        Log(1, std::format("Could not change text: {}", SDL_GetError()));
        return 0;
    }

    CreateLetters(text);

    Log(0, std::format("Successfully changed the text of text at id {}", textId));
    return 1;
}

bool SetTextAttributes(const id_t textId, const std::vector<attributeAndValue>& attributes) {
    if (!texts->contains(textId)) {
        Log(1, std::format("Text at id {} does not exist, cannot set it's attributes", textId));
        return 0;
    }

    Text* text = texts->at(textId);

    bool error = 1;

    std::vector<attributeAndValue> objectAttributes;

    for (attributeAndValue i : attributes) {
        switch (i.first) {
        case OBJECT_ID:
            if (!objects->contains(i.second.asUint())) {
                Log(1, std::format("Object at id {} does not exist, cannot attach a text to it", i.second.asUint()));
                error = 0;
                break;
            }

            text->attachedObjectId = i.second.asUint();
            text->instance         = objects->at(i.second.asUint())->instance;
            break;
        case TEXTURE_ID:
            if (!fonts->contains(i.second.asUint())) {
                Log(1, std::format("Font at id {} does not exist, cannot set it for the text.", i.second.asUint()));
                error = 0;
                break;
            }

            if (!TTF_SetTextFont(text->text, fonts->at(i.second.asUint())->font)) {
                Log(1, std::format("Could not set font at id {} for text at id {}: {}", i.second.asUint(), textId, SDL_GetError()));
                error = 0;
                break;
            }

            fonts->at(text->fontId)->texts.remove(text);

            text->fontId = i.second.asUint();
            fonts->at(text->fontId)->texts.push_back(text);

            break;
        default:
            Log(0, std::format("Attribute type for text does not exist, giving it to the attached Object, if there is one."));
            objectAttributes.push_back(i);
            break;
        }
    }
    if (!objectAttributes.empty()) {
        if (text->attachedObjectId)
            error &= SetObjectAttributes(text->attachedObjectId, objectAttributes);
        else {
            Log(0, "There is no object attached to the text. Creating one.");
            text->attachedObjectId = GetLowestKey(objects, settings::START_SYSTEM_RESOURCES);
            Object* object         = new Object(text->attachedObjectId);
            object->instance       = text->instance;
            error &= SetObjectAttributes(text->attachedObjectId, objectAttributes);
        }
    }

    CreateLetters(text);

    Log(0, std::format("Successfully set attributes for text at id {}", textId));
    return error;
}

Text* GetText(const id_t textId) {
    if (!texts->contains(textId)) {
        Log(1, std::format("Text at id {} does not exist, cannot get it", textId));
        return nullptr;
    }

    return texts->at(textId);
}

} // namespace pg
