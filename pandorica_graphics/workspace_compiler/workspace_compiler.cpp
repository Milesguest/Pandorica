#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <list>
#include <variant>

#include "../include/pg_generic.hpp"
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/variant.hpp>

#include "enums.hpp"

static int8_t valueType = '0';

uint8_t CheckCommand(std::string& command) {
    for (std::pair<std::string, int> cPair : pg::ws::commands) {
        if (cPair.first.compare(command) == 0) {
            return cPair.second;
        }
    }
    return 0;
}

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

    if (val32.type == pg::GenericValue32::Type::Invalid && attribute != pg::ws::PATH) {
        return 0;
    }

    return 1;
}

int main(int argc, char* argv[]) {
    std::list<std::string> paths(argv, argv + argc);
    paths.pop_front();

    std::string basePath = "";

    bool setOutput = 0;
    bool isFolder  = 0;

    for (std::string path : paths) {
        if (path.starts_with("-")) {
            switch (path[1]) {
            case 'o':
                setOutput = 1;
                continue;
            case 'r':
                isFolder = 1;
                continue;
            default:
                std::cout << "ERROR: Unsupported argument.\n";
                return 1;
            }
        }

        if (setOutput) {
            setOutput = 0;
            basePath  = path;
            continue;
        }

        if (isFolder) {
            isFolder = 0;
            if (!std::filesystem::exists(path)) {
                std::cout << std::format("ERROR: Folder at {} does not exist.\n", path);
                continue;
            }

            if (!std::filesystem::is_directory(path)) {
                std::cout << std::format("ERROR: {} is not a folder", path);
                continue;
            }

            for (const auto& file : std::filesystem::directory_iterator(path)) {
                paths.push_back(file.path());
                if (!paths.back().ends_with(".twf"))
                    paths.pop_back();
            }
            continue;
        }

        if (!path.ends_with(".twf"))
            path.append(".twf");
        if (!std::filesystem::exists(path)) {
            std::cout << std::format("ERROR: File at {} does not exist.\n", path);
            continue;
        }

        std::ifstream in(path);

        int           len = path.size() - 4;
        std::ofstream out(std::format("{}/{:.{}}.bwf", basePath, path, len), std::ios::binary);

        cereal::BinaryOutputArchive archive(out);

        bool               eof          = 0;
        bool               insidestring = 0;
        bool               isCommand    = 0;
        bool               afterEqual   = 0;
        pg::GenericValue32 attributeValue;
        std::string        line;
        std::string        command;
        uint8_t            shortValue;
        std::string        attributeName;

        std::vector<std::pair<uint8_t, std::string>> attributes;

        while (!eof) {
            std::getline(in, line);

            if (in.eof()) {
                eof = 1;
                continue;
            }

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
                    if (!pg::ws::attributeNames.contains(attributeName)) {
                        std::cout << std::format("ERROR: Attribute {} does not exist\n", attributeName);
                        break;
                    }

                    attributes.push_back({pg::ws::attributeNames.at(attributeName), ""});

                    afterEqual = 1;
                    continue;
                }

                if (!afterEqual) {
                    attributeName.push_back(letter);
                    continue;
                }

                if (letter == ',') {
                    if (!pg::ws::attributeNames.contains(attributeName)) {
                        std::cout << std::format("ERROR: Attribute {} does not exist\n", attributeName);
                        break;
                    }

                    attributes.push_back({pg::ws::attributeNames.at(attributeName) + attributes.size(), ""});
                    continue;
                }

                attributes.back().second.push_back(letter);
            }

            if (isCommand) {
                if (!(shortValue = CheckCommand(command))) {
                    std::cout << std::format("ERROR: Command !{} does not exist\n", command);
                    continue;
                }

                archive(shortValue);
                continue;
            }

            for (auto attribute : attributes) {
                if (!CheckValue(attribute.first, attribute.second, attributeValue)) {
                    std::cout << std::format("ERROR: Value {} is not supported for {} (check enums.hpp for real name)\n", attribute.second, attribute.first);
                }

                if (attributeValue.type == pg::GenericValue32::Type::Invalid) {
                    archive(static_cast<uint8_t>(0xFF), static_cast<uint8_t>(0x00), attribute.second);
                    continue;
                }

                archive(static_cast<uint8_t>(0xFF), attribute.first, attributeValue);

                continue;
            }
        }
    }
}
