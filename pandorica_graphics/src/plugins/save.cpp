#include "../../include/plugins/save.hpp"
#include "../../include/pg_main.hpp"

namespace pg {
namespace save {

template <typename T>
bool SaveState(T* data, std::string path) {
    if (data == nullptr) {
        Log(1, "Data is null pointer, cannot save it.");
        return 0;
    }

    if (!std::filesystem::exists(std::filesystem::path(path).parent_path())) {
        Log(1, std::format("Path to {} does not exist, cannot save it.", path));
        return 0;
    }

    if (std::filesystem::exists(path)) {
        Log(1, std::format("File at {} already exists, overwriting it.", path));
    }

    std::ofstream out(path, std::ios::binary);
    if (!out.is_open()) {
        Log(1, std::format("Could not open {}, exiting SaveState", path));
        return 0;
    }

    cereal::BinaryOutputArchive archive(out);
    archive(*data);

    Log(0, std::format("Successfully saved state to {}", path));
    return 1;
}

template <typename T>
bool LoadState(T* data, std::string path) {
    if (data == nullptr) {
        Log(1, "Data is null pointer, cannot load to it.");
        return 0;
    }

    if (!std::filesystem::exists(path)) {
        Log(1, std::format("File at {} does not exist, cannot load it.", path));
        return 0;
    }

    std::ifstream in(path, std::ios::binary);
    if (!in.is_open()) {
        Log(1, std::format("Could not open {}, exiting LoadState", path));
        return 0;
    }

    cereal::BinaryInputArchive archive(in);
    archive(*data);

    Log(0, std::format("Successfully loaded state from {}", path));
    return 1;
}

} // namespace save
} // namespace pg
