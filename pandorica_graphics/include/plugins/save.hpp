#pragma once

#include "../pg_generic.hpp"

namespace pg {
namespace save {

/**
 * @brief Saves the given object to a file.
 *
 * Using this function you can save any struct to a file using cereal.
 * The struct only needs a 'serialize' or 'load' and 'save' function.
 * See cereal for more.
 *
 * @note This does not work with normal pg types
 *
 * @param data A pointer to the object to save.
 * @param path The path to save the file to.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @ingroup pg_plugin
 */
template <typename T>
bool SaveState(T* data, std::string path);

/**
 * @brief Loads the given file to an object.
 *
 * Using this function you can load any struct from a file using cereal.
 * The struct only needs a 'serialize' or 'load' and 'save' function.
 * See cereal for more.
 *
 * @note This does not work with normal pg types
 *
 * @param data A pointer to the object to load to.
 * @param path The path to load the file from.
 * @return true if it succeeded, false if it failed. See the logs for more information.
 *
 * @ingroup pg_plugin
 */
template <typename T>
bool LoadState(T* data, std::string path);

} // namespace save
} // namespace pg
