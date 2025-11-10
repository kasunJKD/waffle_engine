#ifndef GLRENDERER_H
#define GLRENDERER_H

#include "assetsManager.h"
#include "entity.h"
#include "glad/glad.h"

struct GLRENDER {
    GLuint VAO, VBO, EBO;
};

void gl_render_init(GLRENDER* gl_render);
void gl_draw(Entity* entity, AssetManager* assets_manager, GLRENDER* gl_render);

void draw_texture(vec3 position, GLuint texture, GLuint shader, GLuint VAO);

#endif //GLRENDERER_H

//RENDER_SYSTEM
//=> will render the entity if rendable
//
//renderSystem {
//  for each entity {
//      if e.flag == rendable {
//          switch(render_type) {
//              e.type:
//                  gldraw(e);
//
//          }
//      }
//  }
//}
//
//gldraw(e) {
//  switch type {
//      .test
//          query shader by name / type
//          bind shader
//          draw shader
//  }
//}
//
//from entities -> add to rendergroup + scratch arena 
//push that group to gl renderer
//reset scratch arena
