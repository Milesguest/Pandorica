/*
 * Note that this is for educational purposes only and cannot be compiled to see something happen.
 * Make sure to include the installed header.
 */
#include <pandorica_graphics/pg.hpp>

/*
 * This tutorial example file will provide you with the basics of pandorica graphics.
 */

// SDL requires you to add argc and argv.
int main(int argc, char* argv[]) {
    // We start by initializing PG
    pg::Init();

    // Then we create the window and gpu.
    pg::CreateWindowAndGPU("PG_Tutorial_01", 640, 360, 0, 1);

    // Now you can create the standard context or
    // decide to develop your own structure.
    pg::CreateStandardContext();

    // After this you can create whatever you like.
    // However you should probably start with a new workspace.
    // Note: For this you best use the workspace compiler with .twf
    pg::CreateWorkspace(1); // We create the workspace at id 1.

    // Now you should probably create 2 transferbuffers.
    // One for uploading textures and the other for uploading instance data.
    pg::CreateTransferBuffer(1, {.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD});
    // You can leave the info.size empty to use the default management type with the default size.

    pg::CreateTransferBuffer(2, {.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD});
    // The texture transferbuffer will be automatically resized to the default texture transferbuffer size when uploading.

    // After creating the transferbuffers, you can create data buffers for them, although this is not necessary.
    // Transferbuffers automatically create a buffer if they don't have an assigned buffer.
    // That buffer will automatically be deleted when deleting the transferbuffer.
    // The automatically created buffer will have an id after START_SYSTEM_RESOURCES.

    // Now you can create textures.
    pg::CreateTexture(1, 2, "example/image1.png");
    pg::CreateTexture(2, 2, "example/image2.png");

    // Now you should upload them.
    pg::UploadTextures(2, 1);
    // Note that 2 is our transferbuffer for uploading textures and 1 is our transferbuffer for uploading instance data.

    // We should probably create objects now.
    pg::CreateObject(1, 1);
    // And also set the attributes.
    pg::SetObjectAttributes(1, {{pg::POSITION_X, 0.1f}, {pg::POSITION_Y, 0.5f}, {pg::WIDTH, 2.0f}});

    pg::CreateObject(2, 2);
    pg::SetObjectAttributes(2, {{pg::POSITION_Z, -0.2f}, {pg::RED, 200}});

    // Now both textures actually have an object assigned to them.
    // We can upload the object data now.
    // You have to do this everytime you create, delete or modify objects before rendering.
    pg::UploadData({1});

    // Now the objects' data also exists on the gpu, however if we were to render now, we'd see nothing.
    // We have to first assign our transferbuffer to the workspace we created.
    pg::SetTransferBufferAttributes(1, {{pg::WORKSPACE_ID, 1}});

    // We also need a camera.
    pg::CreateCamera(1);

    // And make the camera render the workspace.
    pg::SetCameraAttributes(1, {{pg::ADD_WORKSPACE_ID, 1}});

    // Now we can actually render to the screen and see something.
    pg::Render(1);

    return 0;
    // For more information about Pandorica Graphics see the documentation or the next tutorial.
}
