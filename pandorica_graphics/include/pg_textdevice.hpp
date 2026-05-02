#pragma once
#include "pg_font.hpp"
#include "pg_generic.hpp"
#include "pg_text.hpp"

/** @file */
namespace pg {

/**
 * @brief Class used for managing texts.
 *
 * The text device is used for creating texts. It creates a texture atlas containing the letters of the text with the specified font.
 *
 * You have to create this before you can use any texts or fonts.
 *
 * @see Text, Font, CreateTextDevice()
 *
 * @ingroup pg_text
 */
struct TextDevice {
    TTF_TextEngine* textEngine; /**< Pointer to the text engine, handling the texts. */

    idmap<Font> fonts{}; /**< Map of all fonts. */
    idmap<Text> texts{}; /**< Map of all texts. */

    TextDevice();
    ~TextDevice();
};
extern TextDevice* textDevice; /**< Pointer to created TextDevice */

/**
 * @brief Creates a TextDevice
 *
 * With this function you can create the @ref TextDevice.
 *
 * You have to call this function to create any texts or fonts.
 *
 * To destroy the TextDevice you have to run @ref Quit().
 *
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see TextDevice, Quit()
 *
 * @ingroup pg_text
 */
bool CreateTextDevice();

} // namespace pg
