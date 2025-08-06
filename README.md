# Waffle Engine
###### opengl based graphics engine implementing while I'm learning
> [!NOTE]
> main engine is in Inprogress.
---
---
---
## Experiments 
### Simple Raymarching Techniques (experimental) <br>
###### check on branch [raymarching](https://github.com/kasunJKD/waffle_engine/tree/raymarching) 

![image](https://github.com/kasunJKD/waffle_engine/assets/64832263/7f84aaf5-8b56-4763-8852-fb5c7121eddd)

TASK _1 => Hello world triangle render

INPROGRESS
- allocators ---> 1 partial done
- utilise allocators ----> 2 partial done
- defined hashes and utilise them ------> inprogress
- editor section ----------->5
- camera ------->4
- textures - resources --> 6
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
