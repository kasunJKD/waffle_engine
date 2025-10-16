Progress and todos

1. add texture and sprite mapping
2. add camera with types - Debug camera, Player camera, Follow Camera, Room Camera
3. Animations -
4. Collisions -
6. Z-indexing methods
7. RenderGroups and Scene manager and save and load files implementation Basic
7. Sounds
8. Editor with types - sprite browser, set sprites by click and aligned to sections with ruler
9. Editor sprite collider drawer
10.Editor animations editor
11. Editor Empty Scene to save file to loading 
12. OverWorld implementation Editor and Game with spacial rendering techs
13. Room by Room implementation Editor and Game

14.FX and ligting methods 
15.fog of war types and implementation

INPROGRESS
- allocators ---> 1 partial done
- utilise allocators ----> 2 partial done
- defined hashes and utilise them ------> done 
- editor section ----------->5
- camera ------->4
- textures - resources --> done
- shaders - resources ---> done

shader + texture  should be in AssetManager;
renderer get shader/texture by type from assetManager;

asset {
    name
    type
    union {
        GLuint
        TextureId
        ShaderId
    }
}

Startup file
    //startup.dd
    [startup scene]
    default_test 
    [globals]

OverWorld file
    //overworld.dd
    [player_start]
    position=2 3 5
    [scene_transfer]
    position=24 24 55
    scene=test

Scene folder
    //<name>.sd
    [Name]
    test_scene
    [Entities]
    id=1
    position=2.5 2.3 2.5
    
    id=2



