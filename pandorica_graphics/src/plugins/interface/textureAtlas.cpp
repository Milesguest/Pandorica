#include "../../../include/plugins/interface/textureAtlas.hpp"

namespace pg::interface {

pg::idmap<TextureAtlas>* textureAtlases;

TextureAtlas::TextureAtlas(const id_t textureAtlasId) : textureAtlasId(textureAtlasId) {
    (*textureAtlases)[textureAtlasId] = this;
}
TextureAtlas::~TextureAtlas() {
    textureAtlases->erase(textureAtlasId);
}

bool CreateTextureAtlas(id_t textureAtlasId, id_t textureId, pg::vec3f position, pg::vec2f size, const std::vector<std::pair<pg::vec2i, pg::vec4f>>& coordinatesAndCutData) {
    if (!textureAtlasId) {
        pg::Log(1, "TextureAtlasId is not allowed to be 0, cannot create the textureAtlas");
        return 0;
    }

    if (textureAtlases->contains(textureAtlasId)) {
        pg::Log(1, std::format("TextureAtlas with id {} already exists, deleting it", textureAtlasId));
        DeleteTextureAtlas(textureAtlasId);
    }

    if (!pg::GetTexture(textureId)) {
        pg::Log(1, std::format("Could not get texture at id {}, cannot create TextureAtlas", textureId));
        return 0;
    }

    bool error = 1;

    TextureAtlas* textureAtlas = new TextureAtlas(textureAtlasId);

    textureAtlas->textureId = textureId;
    textureAtlas->position  = position;
    textureAtlas->size      = size;

    for (const std::pair<pg::vec2i, pg::vec4f>& i : coordinatesAndCutData) {
        textureAtlas->objects.push_back(pg::GetLowestKey(pg::objects, 1));
        pg::GenericValue32 a;
        error &= pg::CreateObject(textureAtlas->objects.back(), textureId);
        error &= pg::SetObjectAttributes(textureAtlas->objects.back(),
                                         {{pg::POSITION_X, position.x() + i.first.x() * size.x()},
                                          {pg::POSITION_Y, position.y() + i.first.y() * size.y()},
                                          {pg::POSITION_Z, position.z()},
                                          {pg::CUT_X, i.second.x()},
                                          {pg::CUT_Y, i.second.y()},
                                          {pg::CUT_WIDTH, i.second.z()},
                                          {pg::CUT_HEIGHT, i.second.w()}});
    }

    if (!error)
        pg::Log(1, "Problems accured whilst creating the textureAtlas, some data may be incorrect.");

    pg::Log(0, "Successfully created texture atlas");
    return 1;
}

bool DeleteTextureAtlas(id_t textureAtlasId) {
    if (!textureAtlases->contains(textureAtlasId)) {
        pg::Log(1, std::format("TextureAtlas at id {} does not exist", textureAtlasId));
        return 0;
    }

    TextureAtlas* textureAtlas = textureAtlases->at(textureAtlasId);

    bool error = 1;

    for (id_t objectId : textureAtlas->objects) {
        error &= pg::DeleteObject(objectId);
    }

    delete textureAtlas;

    if (!error)
        pg::Log(1, "Problems accured whilst deleting the textureAtlas, some data may be incorrect.");

    pg::Log(0, "Successfully deleted the texture atlas");
    return error;
}

bool AddObjectToTextureAtlas(id_t textureAtlasId, const std::pair<pg::vec2i, pg::vec4f> coordinatesAndCutData) {
    if (!textureAtlases->contains(textureAtlasId)) {
        pg::Log(1, std::format("TextureAtlas at id {} does not exist, cannot add object to texture atlas", textureAtlasId));
        return 0;
    }

    bool error = 1;

    TextureAtlas* textureAtlas = textureAtlases->at(textureAtlasId);

    textureAtlas->objects.push_back(pg::GetLowestKey(pg::objects, 1));

    error &= pg::CreateObject(textureAtlas->objects.back(), textureAtlas->textureId);
    error &= pg::SetObjectAttributes(textureAtlas->objects.back(), {});

    if (!error)
        pg::Log(1, "Problems accured whilst adding an object to the textureAtlas, some data may be incorrect.");

    pg::Log(0, "Successfully added object to texture atlas");
    return error;
}
bool RemoveObjectFromTextureAtlas(id_t textureAtlasId, pg::vec2i coordinate) {
    if (!textureAtlases->contains(textureAtlasId)) {
        pg::Log(1, std::format("TextureAtlas at id {} does not exist, cannot remove an object from it", textureAtlasId));
        return 0;
    }

    TextureAtlas* textureAtlas = textureAtlases->at(textureAtlasId);

    bool error = 1;

    for (auto it = textureAtlas->objects.begin(); it != textureAtlas->objects.end();) {
        id_t objectId = *it;

        if (pg::GetObject(objectId)->instance->position.head(2) ==
            pg::vec2f{
                textureAtlas->position.x() + coordinate.x() * textureAtlas->size.x(),
                textureAtlas->position.y() + coordinate.y() * textureAtlas->size.y()}) {
            error &= pg::DeleteObject(objectId);
            textureAtlas->objects.erase(it);
        } else {
            ++it;
        }
    }

    if (!error)
        pg::Log(1, "Problems accured while deleting an object from the textureAtlas, some data may be incorrect.");

    pg::Log(0, "Successfully removed object from texture atlas");
    return error;
}

TextureAtlas* GetTextureAtlas(id_t textureAtlasId) {
    if (!textureAtlases->contains(textureAtlasId)) {
        pg::Log(1, std::format("TextureAtlas at id {} does not exist, cannot get it", textureAtlasId));
        return nullptr;
    }

    pg::Log(0, std::format("Successfully got textureAtlas at id {}", textureAtlasId));
    return textureAtlases->at(textureAtlasId);
}

} // namespace pg::interface
