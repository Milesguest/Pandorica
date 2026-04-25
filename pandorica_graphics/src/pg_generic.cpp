#include "../include/pg_generic.hpp"
#include "../include/pg_main.hpp"

namespace pg {

namespace settings {
uint32_t
    DEFAULT_TEXTURETRANSFERBUFFER_SIZE = 65536,
    DEFAULT_DYNAMIC_SIZE               = 1680,
    DEFAULT_STATIC_SIZE                = 16800,
    START_SYSTEM_RESOURCES             = 1000,
    LOG_LEVEL                          = 0;
int32_t
    DEFAULT_SAMPLE_COUNT      = 1,
    VSYNC                     = 0,
    COMPRESSED_TEXTURE_FORMAT = -1;
std::string
    LOGFILE_NAME = "pandorica.log";
ManagementType
    DEFAULT_MANAGEMENT_TYPE = MANAGEMENT_DYNAMIC;
SDL_FColor
    CLEAR_COLOR = {0.1f, 0.1f, 0.1f, 1.0f};
} // namespace settings

bool SetHint(SettingsHint hint, GenericValue32 value) {
    switch (hint) {
    case DEFAULT_TEXTURETRANSFERBUFFER_SIZE:
        settings::DEFAULT_TEXTURETRANSFERBUFFER_SIZE = value.asUint();
        break;
    case DEFAULT_DYNAMIC_SIZE:
        settings::DEFAULT_DYNAMIC_SIZE = value.asUint();
        break;
    case DEFAULT_STATIC_SIZE:
        settings::DEFAULT_STATIC_SIZE = value.asUint();
        break;
    case START_SYSTEM_RESOURCES:
        if (textures->contains(settings::START_SYSTEM_RESOURCES)) {
            Log(1, "Not allowed to set START_SYSTEM_RESOURCES after standard context was created.");
            return 0;
        }
        settings::START_SYSTEM_RESOURCES = value.asUint();
        break;
    case LOG_LEVEL:
        if (value.asUint() > 2) {
            Log(1, "LOG_LEVEL cannot be greater than 2.");
            return 0;
        }
        settings::LOG_LEVEL = value.asUint();
        break;
    case DEFAULT_SAMPLE_COUNT:
        if (textures->contains(settings::START_SYSTEM_RESOURCES)) {
            Log(1, "Not allowed to set DEFAULT_SAMPLE_COUNT after standard context was created.");
            return 0;
        }
        settings::DEFAULT_SAMPLE_COUNT = value.asInt();
        break;
    case VSYNC:
        if (!SDL_SetWindowSurfaceVSync(window->window, value.asInt())) {
            Log(1, std::format("Could not set v-sync to {}: {}", value.asInt(), SDL_GetError()));
            return 0;
        }
        settings::VSYNC = value.asBool();
        break;
    case COMPRESSED_TEXTURE_FORMAT:
        if (textures->contains(settings::START_SYSTEM_RESOURCES)) {
            Log(1, "Not allowed to set COMPRESSED_TEXTURE_FORMAT after standard context was created.");
            return 0;
        }
        settings::COMPRESSED_TEXTURE_FORMAT = value.asInt();
    case LOGFILE_NAME:
        if (textures->contains(settings::START_SYSTEM_RESOURCES)) {
            Log(1, "Not allowed to set LOGFILE_NAME after standard context was created.");
            return 0;
        }
        settings::LOGFILE_NAME = value.asString();
        break;
    case DEFAULT_MANAGEMENT_TYPE:
        settings::DEFAULT_MANAGEMENT_TYPE = static_cast<ManagementType>(value.asUint());
        break;
    case CLEAR_COLOR:
        Log(1, "Please directly set CLEAR_COLOR with \'settings::CLEAR_COLOR = VALUE\'");
        return 0;
    }

    Log(0, "Successfully set hint");
    return 1;
}

mat4f CreateProjectionMatrix(const float fovY, const float aspect, const float near, const float far) {
    float tanHalfFovy = tan(fovY / 2.0f);

    mat4f proj = mat4f::Zero();

    proj(0, 0) = 1.0f / (aspect * tanHalfFovy);

    proj(1, 1) = -1.0f / (tanHalfFovy);

    proj(2, 2) = near / (far - near);
    proj(2, 3) = (near * far) / (far - near);

    proj(3, 2) = -1.0f;

    return proj;
}

mat4f Create2DProjectionMatrix(const float fovY, const float aspect) {
    mat4f proj = mat4f::Zero();

    float       height = fovY;
    float       width  = aspect * height;
    const float far    = 1000000.f;
    const float near   = fovY / 2;

    proj(0, 0) = (2.0f * near) / width;
    proj(1, 1) = -(2.0f * near) / height;

    proj(2, 2) = near / (far - near);
    proj(2, 3) = (near * far) / (far - near);

    proj(3, 3) = 1.0f;

    return proj;
}

mat4f CreateViewMatrix(const vec3f& eye, const vec3f& target, const vec3f& up) {
    const vec3f forward = (target - eye).normalized();
    const vec3f right   = up.cross(forward).normalized();
    const vec3f camUp   = forward.cross(right);

    mat4f view             = mat4f::Identity();
    view.block<1, 3>(0, 0) = right.transpose();
    view.block<1, 3>(1, 0) = camUp.transpose();
    view.block<1, 3>(2, 0) = -forward.transpose();
    view.block<3, 1>(0, 3) = vec3f{-right.dot(eye),
                                   -camUp.dot(eye),
                                   forward.dot(eye)};

    return view;
}

mat4f CreateRotationMatrix(const vec3f& rotation) {
    anglef x = {rotation[0], vec3f::UnitX()};
    anglef y = {rotation[1], vec3f::UnitY()};
    anglef z = {rotation[2], vec3f::UnitZ()};

    mat4f rot  = mat4f::Identity();
    mat3f temp = Eigen::Quaternion<float>(x * y * z).matrix();

    rot.block<1, 3>(0, 0) = temp.col(0);
    rot.block<1, 3>(1, 0) = temp.col(1);
    rot.block<1, 3>(2, 0) = temp.col(2);

    return rot;
}

} // namespace pg
