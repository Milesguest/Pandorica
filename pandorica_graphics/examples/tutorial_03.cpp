// Note that this is for educational purposes only and cannot be compiled to see something happen.

#include <pandorica_graphics/pg.hpp>

/*
 * This tutorial file will provide you with information about audio in Pandorica Graphics.
 */

int main(int argc, char* argv[]) {
    // It is assumed you initialized PG, created a window, gpu and the standard context.

    // Audio tracks are part of the audio device. So you first need to create one.
    pg::CreateAudioDevice();
    // An audio device creates an audio stream used for playing audio.

    // After you created the audio device you can load the audio data.
    pg::CreateAudioData(1, "example/scream.mp3");

    // The audio data cannot be played directly, you first need to create an audio track.
    pg::CreateAudioTrack(1, 1);
    // The audio track can be modified in many ways, see SDL3_mixer for more.

    // Audio data can also be part of a workspace, so it will be deleted and loaded with a workspace.

    // For more information about Pandorica Graphics see the documentation or the next tutorial.

    return 0;
}
