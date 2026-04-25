#include "../include/pg_font.hpp"
#include "../include/pg_main.hpp"

namespace pg {

idmap<Font>* fonts;

Font::Font(id_t id) : EngineResource(id) {
    (*fonts)[id] = this;
}
Font::~Font() {
    while (!texts.empty()) {
        delete texts.front();
    }

    TTF_CloseFont(font);
    fonts->erase(id);
}

bool CreateFont(const id_t fontId, const size_t fontSize, const std::string& path) {
    if (!fontId) {
        Log(1, "FontId is not allowed to be 0");
        return 0;
    }

    if (fonts->contains(fontId)) {
        Log(1, std::format("Font at id {} already exists, deleting it", fontId));
        delete fonts->at(fontId);
    }

    Font* font = new Font(fontId);
    font->font = TTF_OpenFont(std::format("{}/{}", cContext->basePath, path).c_str(), fontSize);
    if (!font->font) {
        Log(1, std::format("Could not open font from {}: {}", path, SDL_GetError()));
        return 0;
    }

    Log(0, std::format("Successfully created font at id {}", fontId));
    return 1;
}

bool DeleteFont(const id_t fontId) {
    if (!fonts->contains(fontId)) {
        Log(1, std::format("Font at id {} does not exist, exiting delete function", fontId));
        return 0;
    }

    Font* font = fonts->at(fontId);

    for (Text* text : font->texts) {
        DeleteText(text->id);
    }
    delete font;

    Log(0, std::format("Successfully deleted font at id {}", fontId));
    return 1;
}

Font* GetFont(const id_t fontId) {
    if (!fonts->contains(fontId)) {
        Log(1, std::format("Font at id {} does not exist, cannot get it", fontId));
        return nullptr;
    }

    return fonts->at(fontId);
}

} // namespace pg
