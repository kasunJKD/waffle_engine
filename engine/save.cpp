#include "save.h"
#include "debug.h"
#include "resourceManager.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

void save_game_data(const std::string& filename, EntitySystem* es) {
    std::ifstream fileIn(filename);
    if (!fileIn.is_open()) {
        std::cerr << "Failed to open file for reading.\n";
        return;
    }

    std::vector<std::string> lines;
    std::string line;
    bool found_entities = false;

    while (std::getline(fileIn, line)) {
        lines.push_back(line);

        if (line == "[entities]") {
            found_entities = true;
            break; // Stop here – delete everything after
        }
    }

    fileIn.close();

    if (!found_entities) {
        std::cerr << "[entities] section not found in file.\n";
        return;
    }

    // Add new entity data
    for (const auto& entity : es->entities) {
        if (entity.id) {
                    lines.push_back("id=" + to_string(entity.id));
                    if (entity.type) lines.push_back("type=" + to_string(static_cast<int>(entity.type)));
                    if(entity.worldType) lines.push_back("worldType=" + to_string(static_cast<int>(entity.worldType)));
                    lines.push_back("name=" + string(entity.name));
                    lines.push_back("position=" + 
                        to_string(entity.position.x) + "," +
                        to_string(entity.position.y) + "," +
                        to_string(entity.position.z));
                    if (entity.scale) lines.push_back("scale=" + to_string(entity.scale));
                    if(entity.color.r) lines.push_back("color=" +
                        to_string(entity.color.r) + "," +
                        to_string(entity.color.g) + "," +
                        to_string(entity.color.b));
                    if(entity.texture_name) lines.push_back("texture=" +string(entity.texture_name));
                    if (entity.shader_name) lines.push_back("shader=" + string(entity.shader_name));
                    if(entity.parent_id) lines.push_back("parent_id=" + to_string(entity.parent_id));
                    lines.push_back("active=" + string(entity.active ? "true" : "false"));
                    lines.push_back("");  // blank line between entities
                }
    }

    // Write back to file
    std::ofstream fileOut(filename);
    if (!fileOut.is_open()) {
        std::cerr << "Failed to open file for writing.\n";
        return;
    }

    for (const auto& l : lines) {
        fileOut << l << '\n';
    }

    fileOut.close();
}

// void inline load_texture_data_game(ResourceManager *r) {
//
// }

void inline load_texture_data_game(vector<string> data, ResourceManager *r) {
    DEBUG_LOG("r", r);
    for(const auto& d: data) {
        DEBUG_LOG("texture path %s", d.c_str());
    }
}

void inline load_shader_data_game(vector<string> data, ResourceManager *r) {
    DEBUG_LOG("r", r);
    for(const auto& d: data) {
        DEBUG_LOG("shader path %s", d.c_str());
    }
}
void inline load_font_data_game(vector<string> data, ResourceManager *r) {
    DEBUG_LOG("r", r);
    for(const auto& d: data) {
        DEBUG_LOG("font path %s", d.c_str());
    }
}

void load_game_data(const string& filename, ResourceManager *r) {
    string line;
    ifstream file(filename);
    vector<string> texture_paths;
    vector<string> shader_paths;
    vector<string> font_paths;

    enum Section {
        NONE,
        TEXTURE,
        SHADER,
        FONT,
        ENTITIES
    };

    Section current_section = NONE;

    if (file.is_open()) {
        while(getline(file, line)) {
            if(line.empty()) {
                continue;
            }
            if (line == "[texture path]") {
                    current_section = TEXTURE;
                    continue;
                } else if (line == "[shader path]") {
                    current_section = SHADER;
                    continue;
                } else if (line == "[font path]") {
                    current_section = FONT;
                    continue;
                } else if (line == "[entities]") {
                    current_section = ENTITIES;
                    continue;
                }

                switch (current_section) {
                    case TEXTURE:
                        texture_paths.push_back(line);
                        break;
                    case SHADER:
                        shader_paths.push_back(line);
                        break;
                    case FONT:
                        font_paths.push_back(line);
                        break;
            case ENTITIES:
                    case NONE:
                    default:
                        break;
                }

            }
        file.close();
    }

    if(!texture_paths.empty()) {
        load_texture_data_game(texture_paths, r);
    }
    if(!shader_paths.empty()) {
        load_shader_data_game(shader_paths, r);
    }
    if(!font_paths.empty()) {
        load_font_data_game(font_paths, r);
    }

}
