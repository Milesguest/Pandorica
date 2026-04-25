#include "../include/pg_textdevice.hpp"
#include "../include/pg.hpp"

namespace pg {

TextDevice* textDevice;

TextDevice::TextDevice() {
    textEngine = TTF_CreateGPUTextEngine(gpu->device);
};

TextDevice::~TextDevice() {
    while (!texts.empty()) {
        delete texts.begin()->second;
    }

    while (!fonts.empty()) {
        delete fonts.begin()->second;
    }
}

bool CreateTextDevice() {
    if (cContext->textDevice) {
        Log(1, "TextDevice was already created, deleting the old one");
        delete cContext->textDevice;
    }

    cContext->textDevice = new TextDevice;
    if (!cContext->textDevice->textEngine) {
        Log(2, std::format("Could not create textEngine: {}", SDL_GetError()));
        return 0;
    }
    textDevice = cContext->textDevice;

    texts = &textDevice->texts;
    fonts = &textDevice->fonts;

    return 1;
}

} // namespace pg
