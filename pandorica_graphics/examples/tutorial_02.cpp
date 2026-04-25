// Note that this is for educational purposes only and cannot be compiled to see something happen.

#include <pandorica_graphics/pg.hpp>

/*
 * This tutorial example file will provide you with information about texts in pandorica graphics.
 */

int main(int argc, char* argv[]) {
    // It is assumed you initialized PG, created a window, gpu and the standard context.

    // Texts are part of the transferbuffer and thus you need to first create a transferbuffer.
    pg::CreateTransferBuffer(1, {.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD});

    // After you have your transferbuffer, we can start with the text device
    pg::CreateTextDevice();
    // A text device is required to create texts and fonts. Trying to create fonts or texts without it will crash the programm.

    // Now that we have the text device, we can load a font.
    pg::CreateFont(1, 12, "example/examplefont.ttf");

    // And then create a text using that font.
    pg::CreateText(1, 1, 1);

    // Now we already have a text.
    // To set the text to the string we want we use:
    pg::SetText(1, "This is a tutorial!");
    // Texts are very similar to textures.
    // Internally they even hold a texture. Every letter of the text is basically an object using that texture.

    // To modify the text, we can create an object and link them to eachother, or let the text automatically create one.
    // To make the text create an object and link to them, modify an attribute of the text, that actually modifies the object,
    // e.g.:
    pg::SetTextAttributes(1, {{pg::POSITION_X, 1}});
    // Now we have created an object linked to that text and set its position.x to 1.
    // This will also move the origin of the text by 1 on the x-axis.

    // The origin of texts is always in respect to all letters.

    // For more information about Pandorica Graphics see the documentation or the next tutorial.

    return 0;
}
