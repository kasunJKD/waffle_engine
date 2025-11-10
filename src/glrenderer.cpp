#include "glrenderer.h"

void gl_render_init(GLRENDER* gl_render)
{
    
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };
    
    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glGenVertexArrays(1, &gl_render->VAO);
    glGenBuffers(1, &gl_render->VBO);
    glGenBuffers(1, &gl_render->EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(gl_render->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, gl_render->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_render->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6* sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    
    glBindVertexArray(0); 
}

//redundent code
void gl_draw(Entity* entity, AssetManager* assets_manager, GLRENDER* gl_render)
{
    Asset* test_asset = AssetManager_GetByName(assets_manager, "test_shader");
    Asset* test_texture = AssetManager_GetByName(assets_manager, "image_container");

    switch (entity->type) {
        //test for triangle render
        case T_TEST: {
            glBindTexture(GL_TEXTURE_2D, test_texture->texture.texture);
            glUseProgram(test_asset->shader.program);
            glBindVertexArray(gl_render->VAO); 
            //glDrawArrays(GL_TRIANGLES, 0, 3);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        };break;
    }
}

//TODO render in position need to add
void draw_texture(vec3 position, GLuint texture, GLuint shader, GLuint VAO){
        glBindTexture(GL_TEXTURE_2D, texture);
        glUseProgram(shader);
        glBindVertexArray(VAO); 
            //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
