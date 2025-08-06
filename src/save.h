#ifndef SAVE_H 
#define SAVE_H

#include <string>

enum SaveFileType {
    STARTUP,
    OVERWORLD,
    SCENE
};

struct FileStructure {
    std::string fileName;
    //pointer hash key value pair
};

struct Section {
    std::string header;

};

// //check startup file and what scene or overworld i should load
// inline void load_startup_file(){
//     //check the name with extention 
//     //after check 
//     //load_overworld
//     //load_scene_from scene folder
// };
//
// inline void load_file(std::string path,  SaveFileType fileType) {
// }
//
// inline void save_file(SaveFileType fileType) {
// }


#endif
