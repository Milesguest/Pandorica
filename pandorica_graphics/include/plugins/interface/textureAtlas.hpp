#pragma once

#include "generic.hpp"

namespace pg {
namespace interface {

/** @file */
struct TextureAtlas {
    const id_t textureAtlasId = 0;

    id_t textureId = 0;

    pg::vec3f position{};
    pg::vec2f size{};

    std::vector<id_t> objects{};

    TextureAtlas(const id_t textureAtlasId);
    ~TextureAtlas();
};
extern pg::idmap<TextureAtlas>* textureAtlases;

bool          CreateTextureAtlas(id_t textureAtlasId, id_t pgTextureId, pg::vec3f position, pg::vec2f size, const std::vector<std::pair<pg::vec2i, pg::vec4f>>& coordinatesAndCutData);
bool          DeleteTextureAtlas(id_t textureAtlasId);
bool          AddObjectToTextureAtlas(id_t textureAtlasId, const std::pair<pg::vec2i, pg::vec4f> coordinatesAndCutData);
bool          RemoveObjectFromTextureAtlas(id_t textureAtlasId, pg::vec2i coordinate);
TextureAtlas* GetTextureAtlas(id_t textureAtlasId);

} // namespace interface
} // namespace pg
