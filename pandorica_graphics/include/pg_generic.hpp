#pragma once

#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Eigen>
#include <eigen3/Eigen/Geometry>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <array>
#include <cfloat>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <string>

/** @file */
namespace pg {
struct GenericValue32;

using id_t   = uint32_t;
using size_t = uint32_t;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef MAXFLOAT
#define MAXFLOAT 3.40282347e+38F
#endif

#ifdef _WIN32
#undef near
#undef far
#undef min
#undef max
#endif

/**
 * @brief How transfer-/Buffers should be handled
 *
 * See the specific types for more information.
 *
 * @see Buffer, TransferBuffer
 *
 * @ingroup pg_main
 */
enum ManagementType {
    MANAGEMENT_UNDEFINED, /**< Do not automatically resize buffers. */
    MANAGEMENT_STATIC,    /**< Automatically resize buffers to 'n * DEFAULT_STATIC_SIZE', but only upsizes. */
    MANAGEMENT_DYNAMIC    /**< Automatically resize buffers to 'n * DEFAULT_DYNAMIC_SIZE', sizes up and down. */
};

namespace settings {
extern uint32_t
    DEFAULT_TEXTURETRANSFERBUFFER_SIZE,
    DEFAULT_DYNAMIC_SIZE,
    DEFAULT_STATIC_SIZE,
    START_SYSTEM_RESOURCES,
    LOG_LEVEL;
extern int32_t
    DEFAULT_SAMPLE_COUNT,
    VSYNC,
    COMPRESSED_TEXTURE_FORMAT;
extern std::string
    LOGFILE_NAME;
extern ManagementType
    DEFAULT_MANAGEMENT_TYPE;
extern SDL_FColor
    CLEAR_COLOR;
} // namespace settings
/**
 * @brief Settings which can be adjusted
 * Please only set the settings using the 'SetHint()' function if not
 * specified otherwise.
 * Some settings can only be adjusted before the standard context.
 *
 * Settings are located in the 'pg::settings' namespace.
 *
 * @see SetHint()
 *
 * @ingroup pg_main
 */
enum SettingsHint {
    DEFAULT_TEXTURETRANSFERBUFFER_SIZE, /** The default size for transferBuffers used for texture transfer. */
    DEFAULT_DYNAMIC_SIZE,               /**< The default size for transfer-/Buffers which are handled dynamically. They will be down- and upsized */
    DEFAULT_STATIC_SIZE,                /**< The default size for transfer-/Buffers which are used with a lot of data. They will only be upsized */
    START_SYSTEM_RESOURCES,             /**< Don't change after standard context was created. The id at which system resources start */
    LOG_LEVEL,                          /**< The log messages to output in the logs file. 0 for everything, 1 for warnings and errors, 2 for errors */
    DEFAULT_SAMPLE_COUNT,               /**< Don't change after standard context was created. The default mulitsample count should be 1, 2, 4 or 8. Defaults to 1. Is not normally used in 'Render()'. */
    VSYNC,                              /**< Don't change after standard context was created. How v-sync should be implemented. See 'SDL_GPUPresentMode' for more info. If the current present mode is unsupported by the window, v-sync will automatically be enabled. */
    COMPRESSED_TEXTURE_FORMAT,          /**< Don't change after standard context was created. The compression format for textures. -1 to not compress textures. If texture format is unsupported by the driver, uncompressed texture formats will be used. */
    LOGFILE_NAME,                       /**< Don't change after standard context was created. The name of the log file. */
    DEFAULT_MANAGEMENT_TYPE,            /**< The default management type to use for transfer-/Buffers. Defaults to MANAGEMENT_DYNAMIC */
    CLEAR_COLOR                         /**< The color to clean the window texture with before rendering. Please directly set this with 'settings::CLEAR_COLOR = VALUE'. */
};
/**
 * @brief Use to set settings for the engine.
 *
 * @note Some settings can only be adjusted before creating the standard context.
 *
 * @param hint The setting to change.
 * @param value The new value of the setting.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @see SettingsHint
 *
 * @ingroup pg_main
 */
bool SetHint(SettingsHint hint, GenericValue32 value);

/**
 * @brief Changeable attributes for engine ressources
 *
 * @ingroup pg_main
 */
enum Attribute {
    POSITION_X,            ///< The X-coordinate [-FLOAT_MAX; FLOAT_MAX]
    R_POSITION_X,          ///< The relative X-coordinate [-FLOAT_MAX; FLOAT_MAX]
    POSITION_Y,            ///< The Y-coordinate [-FLOAT_MAX; FLOAT_MAX]
    R_POSITION_Y,          ///< The relative Y-coordinate [-FLOAT_MAX; FLOAT_MAX]
    POSITION_Z,            ///< The Z-coordinate [-FLOAT_MAX; FLOAT_MAX]
    R_POSITION_Z,          ///< The relative Z-Coordinate [-FLOAT_MAX; FLOAT_MAX]
    WIDTH,                 ///< The width [0; FLOAT_MAX]
    R_WIDTH,               ///< The relative width [0; FLOAT_MAX]
    HEIGHT,                ///< The height [0; FLOAT_MAX]
    R_HEIGHT,              ///< The relative height [0; FLOAT_MAX]
    ROTATION_X,            ///< The X-axis rotation (in degrees) [-FLOAT_MAX; FLOAT_MAX]
    R_ROTATION_X,          ///< The relative X-axis rotation (in degrees) [-FLOAT_MAX; FLOAT_MAX]
    ROTATION_Y,            ///< The Y-axis rotation (in degrees) [-FLOAT_MAX; FLOAT_MAX]
    R_ROTATION_Y,          ///< The relative Y-axis rotation (in degrees) [-FLOAT_MAX; FLOAT_MAX]
    ROTATION_Z,            ///< The Z-axis rotation (in degrees) [-FLOAT_MAX; FLOAT_MAX]
    R_ROTATION_Z,          ///< The relative Z-axis rotation (in degrees) [-FLOAT_MAX; FLOAT_MAX]
    CUT_X,                 ///< The X-coordinate of the cut position [0; 1]
    R_CUT_X,               ///< The relative X-coordinate of the cut position [0; 1]
    CUT_Y,                 ///< The Y-coordinate of the cut position [0; 1]
    R_CUT_Y,               ///< The relative Y-coordinate of the cut position [0; 1]
    CUT_WIDTH,             ///< The width of the cut rectangle [0; 1]
    R_CUT_WIDTH,           ///< The relative width of the cut rectangle [0; 1]
    CUT_HEIGHT,            ///< The height of the cut rectangle [0; 1]
    R_CUT_HEIGHT,          ///< The relative height of the cut rectangle [0; 1]
    ORIGIN_X,              ///< The X-coordinate of the origin [0; 1] or outside the instance [-FLOAT_MAX; FLOAT_MAX]
    R_ORIGIN_X,            ///< The relative X-coordinate of the origin [0; 1] or outside the instance [-FLOAT_MAX; FLOAT_MAX]
    ORIGIN_Y,              ///< The Y-coordinate of the origin [0; 1] or outside the instance [-FLOAT_MAX; FLOAT_MAX]
    R_ORIGIN_Y,            ///< The relative Y-coordinate of the origin [0; 1] or outside the instance [-FLOAT_MAX; FLOAT_MAX]
    ORIGIN_Z,              ///< The Z-coordinate of the origin [0; 1] or outside the instance [-FLOAT_MAX; FLOAT_MAX]
    R_ORIGIN_Z,            ///< The relative Z-coordinate of the origin [0; 1] or outside the instance [-FLOAT_MAX; FLOAT_MAX]
    RED,                   ///< How much red will be blocked (8bit) [0; 255]
    R_RED,                 ///< How much red will be relatively blocked (8bit) [0; 255]
    GREEN,                 ///< How much green will be blocked (8bit) [0; 255]
    R_GREEN,               ///< How much green will be relativeley blocked (8bit) [0; 255]
    BLUE,                  ///< How much blue will be blocked (8bit) [0; 255]
    R_BLUE,                ///< How much blue will be relatively blocked (8bit) [0; 255]
    ALPHA,                 ///< How much alpha will be blocked (8bit) [0; 255]
    R_ALPHA,               ///< How much alpha will be relatively blocked (8bit) [0; 255]
    TEXTURE_ID,            ///< The id of the texture to use [0; 2^32]
    ENABLED,               ///< Whether or not something should be drawn {true, false}
    TRANSFERBUFFER_ID,     ///< The id of the transferbuffer to use [0; 2^32]
    TRANSFERBUFFER_OFFSET, ///< The offset inside the transferbuffer to use (ignored) [0; 2^32]
    WORKSPACE_ID,          ///< The id of the workspace to be part of [0; 2^32]
    SAMPLER_ID,            ///< The id of the sampler to use (ignored) [0; 2^32]
    INSTANCEBUFFER_ID,     ///< The id of the buffer to use [0; 2^32]
    INSTANCEBUFFER_OFFSET, ///< The offset inside the buffer to use [0; 2^32]
    MANAGEMENT_TYPE,       ///< The type of management to use (see @ref ManagementType)
    TARGET_X,              ///< The X-coordinate of the point to look at [-FLOAT_MAX; FLOAT_MAX]
    R_TARGET_X,            ///< The relative X-coordinate of the point to look at [-FLOAT_MAX; FLOAT_MAX]
    TARGET_Y,              ///< The Y-coordinate of the point to look at [-FLOAT_MAX; FLOAT_MAX]
    R_TARGET_Y,            ///< The relative Y-coordinate of the point to look at [-FLOAT_MAX; FLOAT_MAX]
    TARGET_Z,              ///< The Z-coordinate of the point to look at [-FLOAT_MAX; FLOAT_MAX]
    R_TARGET_Z,            ///< The relative Z-coordinate of the point to look at [-FLOAT_MAX; FLOAT_MAX]
    FOV_Y,                 ///< The field of view on the Y-axis (in degrees) [0; 360]
    ASPECT_RATIO,          ///< The aspect ratio of the camera ]0; FLOAT_MAX]
    DDD,                   ///< 3D
    ADD_WORKSPACE_ID,      ///< Add a workspace to a camera [0; 2^32]
    REMOVE_WORKSPACE_ID,   ///< Remove a workspace from a camera [0; 2^32]
    OBJECT_ID              ///< The id of the object to use [0; 2^32]
};

typedef Eigen::Vector2f           vec2f;
typedef Eigen::Vector2i           vec2i;
typedef Eigen::Vector3f           vec3f;
typedef Eigen::Vector3d           vec3d;
typedef Eigen::Vector4f           vec4f;
typedef Eigen::Vector<uint8_t, 4> vec4i;
typedef Eigen::Matrix3f           mat3f;
typedef Eigen::Matrix3d           mat3d;
typedef Eigen::Matrix4f           mat4f;
typedef Eigen::Matrix4d           mat4d;
typedef Eigen::AngleAxisf         anglef;
typedef Eigen::AngleAxisd         angled;
template <typename T>
using idmap             = std::unordered_map<id_t, T*>;
using attributeAndValue = std::pair<Attribute, GenericValue32>;

/**
 * @brief A struct used for engine resources.
 *
 * @ingroup pg_main
 */
struct EngineResource {
    const id_t id; /**< The id of the specific engine resource. */
    EngineResource(id_t id) : id(id) {};
};

/**
 * @brief A struct specifying the layout of a vertex.
 *
 * @ingroup pg_main
 */
struct Vertex {
    vec3f position;
    vec2f uv;
};

/**
 * @brief A struct specifying the layout of an instance.
 *
 * @ingroup pg_main
 */
struct Instance {
    vec3f    position = {0.f, 0.f, 0.f};
    vec2f    size     = {1.0f, 1.0f};
    vec3f    origin   = {0.f, 0.f, 0.f};
    vec3f    rotationX{};
    vec3f    rotationY{};
    vec3f    rotationZ{};
    uint32_t color{};
    vec2f    cutPos{0.f, 0.f};
    vec2f    cutSize = {1.0f, 1.0f};
};

/**
 * @brief A struct which can hold multiple 32bit mathematical types, as well as 'const char*'.
 *
 * @note This is not used in any structs and just as parameters.
 *
 * @ingroup pg_main
 */
struct GenericValue32 {
    union {
        float       f;
        uint32_t    u;
        int32_t     i;
        bool        b;
        const char* s;
    };

    enum class Type : uint8_t { Invalid,
                                Float,
                                Uint,
                                Int,
                                Bool,
                                String };
    Type type;

    GenericValue32() : u(0), type(Type::Uint) {}
    GenericValue32(float v) { *this = v; }
    GenericValue32(uint32_t v) { *this = v; }
    GenericValue32(int32_t v) { *this = v; }
    GenericValue32(bool v) { *this = v; }
    GenericValue32(const char* v) { *this = v; }

    GenericValue32& operator=(float v) {
        f    = v;
        type = Type::Float;
        return *this;
    }
    GenericValue32& operator=(uint32_t v) {
        u    = v;
        type = Type::Uint;
        return *this;
    }
    GenericValue32& operator=(int32_t v) {
        i    = v;
        type = Type::Int;
        return *this;
    }
    GenericValue32& operator=(bool v) {
        b    = v;
        type = Type::Bool;
        return *this;
    }
    GenericValue32& operator=(const char* v) {
        s    = v;
        type = Type::String;
        return *this;
    }

    template <typename T>
    explicit operator T() const {
        static_assert(sizeof(T) <= 4 || type == Type::String, "Target type is too large");
        switch (type) {
        case Type::Invalid:
            return static_cast<T>(NULL);
        case Type::Float:
            return static_cast<T>(f);
        case Type::Uint:
            return static_cast<T>(u);
        case Type::Int:
            return static_cast<T>(i);
        case Type::Bool:
            return static_cast<T>(b);
        case Type::String:
            if constexpr (std::is_same_v<T, float>)
                return s ? static_cast<T>(std::atof(s)) : 0.0f;
            else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>)
                return s ? static_cast<T>(std::atoi(s)) : 0;
            else if constexpr (std::is_same_v<T, bool>)
                return s != nullptr && s[0] != '\0';
            else
                return T{};
        }

        std::unreachable();
    }

    float asFloat() const {
        return static_cast<float>(*this);
    };
    uint32_t asUint() const {
        return static_cast<uint32_t>(*this);
    };
    int32_t asInt() const {
        return static_cast<int32_t>(*this);
    };
    bool asBool() const {
        return static_cast<bool>(*this);
    };
    std::string asString() const {
        return type == Type::String ? s : "";
    }

    template <class Archive>
    void serialize(Archive& archive) {
        archive(type, u);
    }
};

/* Generic functions */

/**
 * @brief Finds the lowest key in the specified list.
 *
 * @param mapToLookIn The map to look in.NO
 * @param startValue The value at which to start searching.
 * @return The lowest key of the map starting from 'startValue'.
 *
 * @ingroup pg_main
 */
template <typename TMapType>
inline id_t GetLowestKey(const idmap<TMapType>* mapToLookIn, id_t startValue) {
    while (true) {
        if (!mapToLookIn->contains(startValue))
            return startValue;

        ++startValue;
    }
}

/**
 * @internal Create a 3D Projection Matrix.
 *
 * @ingroup pg_main
 */
mat4f CreateProjectionMatrix(const float fovY, const float aspect, const float near, const float far);
/**
 * @internal Create a 2D Projection Matrix.
 *
 * @ingroup pg_main
 */
mat4f Create2DProjectionMatrix(const float fovY, const float aspect);
/**
 * @internal Create a view matrix.
 *
 * @ingroup pg_main
 */
mat4f CreateViewMatrix(const vec3f& eye, const vec3f& target, const vec3f& up);
/**
 * @internal Create a rotation matrix.
 *
 * @ingroup pg_main
 */
mat4f CreateRotationMatrix(const vec3f& rotation);

} // namespace pg
