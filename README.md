# Pandorica Graphics
## What is Pandorica Graphics?
Pandorica Graphics is a WIP graphics engine optimized for speed. <br>
It supports 3D and 2D, however everything in the engine is a 2D surface (plain). In the
3D context, these plains can be turned, scaled and moved around.<br>
The engine is highly customizable and works out of the box. To modify the engine to
you liking, you would not use the predefined "main"-functions and instead develop your
own.<br>
Pandorica Graphics is very low-level and is directly using vulkan, directX, metal, etc. 
through SDL3 and is thus compatible with many platforms.<br>
## Getting started
### Requirements
SDL3, SDL3_image, SDL3_ttf, SDL3_mixer and optionally SDL3_shadercross.<br>
### Installing
Download the binaries and after that include the files in 
your project. <br>
### How it works
Pandorica Graphics consists of 2 types: devices and resources.<br>
Devices can only exist once per program and have to be initialized to use. These devices
then form a 'context'. Subordinated to these devices are the resources. They can be objects,
textures, shaders and many more. Consult the [documentation](https://milesguest.github.io/Pandorica/index.html) (or the header file of the specific 
type) for more information.<br><br>
### How to use it
To use Pandorica Graphics the following steps have to be done pretty much all the time:<br>
- Initialize the context with '@ref pg::Init()'.
- Create window and GPU with '@ref pg::CreateWindowAndGPU()'.
- If you plan to use the built-in rendering process run '@ref pg::CreateStandardContext()'.

### Diagram of types
For the context:
- Context
  - GPUDevice
    - Textures
    - Buffers
    - TransferBuffers
    - Pipelines
    - Shaders
    - Samplers
  - Window
    - Objects
    - Cameras
    - Workspaces
  - TextDevice
    - Fonts
    - Texts
  - AudioDevice
    - AudioData
    - AudioTracks
   
And for the workspaces:
- Workspace
  - TransferBuffers
    - Textures
      - Objects
    - Texts
  - AudioData
    - AudioTracks
