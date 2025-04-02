#include "save.h"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

void save_game_data(const std::string& filename, EntitySystem *es) {
    ifstream fileIn(filename);
    if (!fileIn.is_open()) {
            cerr << "Failed to open file for reading.\n";
            return;
    }

    vector<std::string> lines;
    string line;
    bool inserted = false;

    while (getline(fileIn, line)) {
        lines.push_back(line);
        if (!inserted && line == "[entities]") {
            // Insert new data after [entities]
            for (const auto& data : es->entities) {
                if(data.id) {
                    lines.push_back(to_string(data.id));
                }
            }
            inserted = true;
        }
    }
    
    fileIn.close();
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
