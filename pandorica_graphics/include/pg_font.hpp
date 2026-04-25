#pragma once
#include "pg_generic.hpp"
#include "pg_text.hpp"

/** @file */
namespace pg {

/**
 * @brief A font used for creating texts.
 *
 * Fonts are part of the text device and are referenced by texts.
 *
 * @see TextDevice, Text, CreateFont(), DeleteFont(), GetFont()
 *
 * @ingroup pg_text
 */
struct Font : EngineResource {
    TTF_Font* font; /**< A pointer to the actual font (SDL_ttf). */

    std::list<Text*> texts; /**< A list of texts created with this font. */

    Font(id_t id);
    ~Font();
};
extern idmap<Font>* fonts; /** A pointer to 'textDevice.fonts'. */

/**
 * @brief Create a font.
 *
 * @param fontId The id at which to create the font at.
 * @param fontSize The point size of the font.
 * @param path The file where the font is located.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Font, DeleteFont()
 *
 * @ingroup pg_text
 */
bool CreateFont(const id_t fontId, const size_t fontSize, const std::string& path);
/**
 * @brief Delete a font.
 *
 * @note Texts created with this font will also be deleted.
 *
 * @param fontId The id of the font to delete.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Font, CreateFont()
 *
 * @ingroup pg_text
 */
bool DeleteFont(const id_t fontId);
/**
 * @brief Get a font.
 *
 * @param fontId The id of the font to return.
 * @return A pointer to the font on success or NULL on failure.
 *
 * @see Font
 *
 * @ingroup pg_text
 */
Font* GetFont(const id_t fontId);

} // namespace pg
