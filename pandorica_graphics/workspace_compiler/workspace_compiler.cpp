/*
 * Just compile this file and look at the binary.
 */

#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <list>
#include <variant>

#include <pandorica_graphics/pg.hpp>

#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/variant.hpp>

#include <CLI/CLI.hpp>

#include "commands.hpp"
#include "enums.hpp"

int8_t valueType = '0';

pg::GenericValue32 parseString(std::string_view sv) {
    if (sv.back() == 'f' || sv.back() == 'F') {
        sv.remove_suffix(1);
    }

    bool hasDecimal = (sv.find('.') != std::string_view::npos ||
                       sv.find('e') != std::string_view::npos ||
                       sv.find('E') != std::string_view::npos);

    if (hasDecimal) {
        float val;

        auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), val);
        if (ec == std::errc{} && ptr == sv.data() + sv.size()) return val;
    } else {
        int64_t val;

        auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), val);

        if (ec == std::errc{} && ptr == sv.data() + sv.size()) {
            if (val < 0) {
                return static_cast<int32_t>(val);
            }
            return static_cast<uint32_t>(val);
        }
    }

    pg::GenericValue32 err = 0;

    err.type = pg::GenericValue32::Type::Invalid;

    return err;
}

bool CheckValue(int16_t attribute, std::string& value, pg::GenericValue32& val32) {
    val32 = parseString(value);

    if (val32.type == pg::GenericValue32::Type::Invalid && pg::ws::GetPGByAttribute(static_cast<pg::ws::Attr>(attribute)) != -2) {
        return 0;
    }

    return 1;
}

int main(int argc, char* argv[]) {
    CLI::App app{"Pandorica Graphics Workspace Compiler"};

    std::vector<std::string> paths;
    std::string              basePath;
    bool                     isFolder;

    app.add_option("-o", basePath, "Set output directory");
    app.add_flag("-r", isFolder, "Use folders as input");
    app.add_option("paths", paths, "The paths for the input files");

    CLI11_PARSE(app, argc, argv);

    std::vector<std::string> folders;

    if (isFolder) {
        basePath.append("/");

        folders = paths;
        paths.clear();

        for (std::string folder : folders) {
            if (!std::filesystem::exists(folder) || !std::filesystem::is_directory(folder)) {
                std::cout << std::format("ERROR: Directory at {} does not exist or is not a folder.\n", folder);
                continue;
            }

            for (const auto& file : std::filesystem::directory_iterator(folder)) {
                if (file.is_regular_file()) {
                    paths.push_back(file.path().string());
                }
            }
        }
    }

    for (std::string path : paths) {
        if (!path.ends_with(".twf"))
            path.append(".twf");
        if (!std::filesystem::exists(path)) {
            std::cout << std::format("ERROR: File at {} does not exist.\n", path);
            continue;
        }

        std::ifstream in(path);

        if (isFolder || !basePath.empty()) {
            path = std::filesystem::path(path).filename();
        }

        int           len = path.size() - 4;
        std::ofstream out(std::format("{}{}.bwf", basePath, std::filesystem::path(path).stem().string()), std::ios::binary);

        if (!out.is_open()) {
            std::cout << std::format("ERROR: Could not set output directory: {}\n", basePath);
            continue;
        }

        cereal::PortableBinaryOutputArchive archive(out);

        bool               eof          = 0;
        bool               insidestring = 0;
        bool               isCommand    = 0;
        bool               afterEqual   = 0;
        pg::GenericValue32 attributeValue;
        std::string        line;
        std::string        command;
        std::string        attributeName;

        std::vector<std::pair<uint8_t, std::string>> attributes;

        while (std::getline(in, line)) {
            command.clear();
            attributes.clear();
            attributeName.clear();
            isCommand    = 0;
            insidestring = 0;
            afterEqual   = 0;

            for (char& letter : line) {
                if (insidestring && afterEqual) {
                    if (letter == '\"') {
                        insidestring = 0;
                        continue;
                    }
                    attributes.back().second.push_back(letter);
                    continue;
                }

                if (letter == '#')
                    break;

                if (letter == '\"' && afterEqual) {
                    insidestring = 1;
                    continue;
                }

                if (letter == ' ')
                    continue;

                if (letter == '!') {
                    isCommand = 1;
                    continue;
                }

                if (isCommand) {
                    command.push_back(letter);
                    continue;
                }

                if (letter == '=') {
                    auto attr = pg::ws::GetAttributeByName(attributeName);
                    if (!attr) {
                        std::cout << std::format("ERROR: Attribute {} does not exist\n", attributeName);
                        break;
                    }

                    attributes.push_back({static_cast<uint8_t>(*attr), ""});

                    afterEqual = 1;
                    continue;
                }

                if (!afterEqual) {
                    attributeName.push_back(letter);
                    continue;
                }

                if (letter == ',') {
                    auto attr = pg::ws::GetAttributeByName(attributeName);
                    if (!attr) {
                        std::cout << std::format("ERROR: Attribute {} does not exist\n", attributeName);
                        break;
                    }

                    attributes.push_back({static_cast<uint8_t>(*attr) + attributes.size(), ""});

                    continue;
                }

                attributes.back().second.push_back(letter);
            }

            if (isCommand) {
                auto commandValue = pg::ws::GetCommandByName(command);
                if (!commandValue.has_value()) {
                    std::cout << std::format("ERROR: Command !{} does not exist (check commands.hpp)\n", command);
                    continue;
                }

                archive(static_cast<uint8_t>(commandValue.value()));
                continue;
            }

            for (auto attribute : attributes) {
                if (!CheckValue(attribute.first, attribute.second, attributeValue)) {
                    std::cout << std::format("ERROR: Value {} is not supported for {} (check enums.hpp)\n", attribute.second, attribute.first);
                    continue;
                }

                if (attributeValue.type == pg::GenericValue32::Type::Invalid) {
                    archive(static_cast<uint8_t>(0xFF), attribute.first, attribute.second);
                    continue;
                }

                archive(static_cast<uint8_t>(0xFF), attribute.first, attributeValue);

                continue;
            }
        }
    }
}
