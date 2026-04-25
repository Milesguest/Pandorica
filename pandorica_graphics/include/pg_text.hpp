#pragma once
#include "pg_generic.hpp"

/** @file */
namespace pg {

/**
 * @brief A text which can be drawn.
 *
 * The letters, TTF_Text and letterAtlas hold the textual data and the other attributes are used to create that data.
 * To manipulate specific attributes unique for texts use 'SDL3_ttf'.
 *
 * Texts share their instance with an object but many attributes will not be used for texts.
 *
 * Every letter of the text will be drawn as an instance of the texture atlas.
 *
 * Texts are part of the @ref TextDevice and are referenced by transferbuffers and fonts.
 *
 * @see TextDevice, CreateText(), DeleteText(), SetText(), SetTextAttributes(), GetText()
 *
 * @ingroup pg_text
 */
struct Text : EngineResource {
    TTF_Text* text; /**< Pointer to the actual text. */

    SDL_GPUTexture* letterAtlas; /**< Texture atlas of all the required letters. */

    std::shared_ptr<Instance> instance; /**< Shared instance with an object. */

    std::vector<Instance> letters; /**< Vector of all the letters. */

    id_t fontId;           /**< The id of the font this text is using. */
    id_t attachedObjectId; /**< Id of the object the text is attached to. */
    id_t transferBufferId; /**< Id of the transferbuffer the text is assigned to. */
    bool enabled = 1;      /**< Whether or not the text should be drawn. */

    Text(id_t id, id_t fontId, id_t transferBufferId);
    ~Text();

    /** @internal */
    template <class Archive>
    void serialize(Archive& archive) {
        archive(std::string(text->text).c_str(), attachedObjectId, enabled);
    }
};
extern idmap<Text>* texts; /**< Pointer to 'textDevice->texts'. */

/**
 * @brief Creates a text.
 *
 * Creates a text at the specified id with the specified font assigned to the specified transferbuffer.
 *
 * @param textId The id at which to create the text.
 * @param fontId The id of the font the text should use.
 * @param transferBufferId The id of the transferbuffer the text should be assigned to.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Text, Font, TransferBuffer, DeleteText()
 *
 * @ingroup pg_text
 */
bool CreateText(const id_t textId, const id_t fontId, const id_t transferBufferId);
/**
 * @brief Deletes a text.
 *
 * @param textId The id of the text to delete.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Text, CreateText()
 *
 * @ingroup pg_text
 */
bool DeleteText(const id_t textId);
/**
 * @brief Set the string data for a text.
 *
 * With this function you can change the string the text should use.
 *
 * @param textId The id of the text to alter.
 * @param textData The new string data for the text.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see Text
 *
 * @ingroup pg_text
 */
bool SetText(const id_t textId, const std::string& textData);
/**
 * @brief Set the attributes of a text.
 *
 * Supported attribute(s): \n
 * OBJECT_ID, TEXTURE_ID (fontId), (*)POSITION_X, (*)R_POSITION_X, (*)POSITION_Y, (*)R_POSITION_Y, (*)POSITION_Z, (*)R_POSITION_Z, (*)ROTATION_X, (*)R_ROTATION_X, (*)ROTATION_Y, (*)R_ROTATION_Y, (*)ROTATION_Z, (*)R_ROTATION_Z, (*)RED, (*)R_RED, (*)GREEN, (*)R_GREEN, (*)BLUE, (*)R_BlUE, (*)ALPHA, (*)R_ALPHA, (*)ORIGIN_X, (*)R_ORIGIN_X, (*)ORIGIN_Y, (*)R_ORIGIN_Y
 *
 * (*) = actually modifying the attached object.
 *
 * To change the string data, you have to use @ref SetText().
 *
 * @note Every attribute that works on objects will not throw an error, however they will not change anything about the text.
 *
 * @see Text, Object, SetText()
 *
 * @ingroup pg_text
 */
bool SetTextAttributes(const id_t textId, const std::vector<attributeAndValue>& attributes);
/**
 * @brief Get a text.
 *
 * @param textId The id of the text to return.
 * @return A pointer to the text on success or NULL on failure. See the logs for more information.
 *
 * @see Text
 *
 * @ingroup pg_text
 */
Text* GetText(const id_t textId);

} // namespace pg
