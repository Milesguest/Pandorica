#pragma once

#include "cereal/archives/portable_binary.hpp"
#include "commands.hpp"
#include "enums.hpp"
#include <pandorica_graphics/pg.hpp>

namespace pg {
namespace ws {

inline bool CreateResource(uint8_t                                     command,
                           pg::vector<std::pair<int, GenericValue32>>& attributes,
                           pg::vector<std::pair<int, std::string>>&    attributesString) {
    bool error = 1;

    auto funcPtr = GetFunctionByCommand(static_cast<ws::Command>(command));
    if (!funcPtr) {
        Log(1, std::format("Could not get function of command {}", command));
        return 0;
    }

    error &= funcPtr(attributes, attributesString);

    attributes.clear();
    attributesString.clear();

    if (!error)
        Log(1, std::format("Some problems accured with command {}, some data may be incorrect", command));
    return error;
}

inline bool LoadBWF(std::string_view path) {
    if (!std::filesystem::exists(std::format("{}/{}", cContext->basePath, path))) {
        Log(1, std::format("File at {} does not exist, cannot load workspace", path));
        return 0;
    }

    bool error = 1;

    std::ifstream in(std::format("{}/{}", cContext->basePath, path), std::ios::binary);

    cereal::PortableBinaryInputArchive archive(in);

    bool    eof     = 0;
    uint8_t command = 0;
    uint8_t attName = 0;
    uint8_t letter  = 0;

    GenericValue32 value;
    std::string    valueString;

    pg::vector<std::pair<int, GenericValue32>> attributes{};
    pg::vector<std::pair<int, std::string>>    attributesString{};

    while (!eof) {
        in.peek();
        if (in.eof()) {
            eof = 1;

            error &= CreateResource(command, attributes, attributesString);
            in.close();
            break;
        }

        archive(letter);

        if (letter < 0xFF) {
            if (command) {
                error &= CreateResource(command, attributes, attributesString);
            }
            command = letter;
            continue;
        }

        archive(attName);

        if (GetPGByAttribute(static_cast<ws::Attr>(attName)) == -2) {
            archive(valueString);
            attributesString.push_back({attName, valueString});
        } else {
            archive(value);
            attributes.push_back({attName, value});
        }
    }

    if (error)
        Log(1, std::format("Some problems accured while loading {}, some data may be incorrect", path));
    Log(0, std::format("Successfully loaded {}", path));
    return error;
}

} // namespace ws
} // namespace pg
