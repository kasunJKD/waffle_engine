//have what we need to render on screen like a struct
//calculation of vertices
//load shaders
//create and link program
//use program
//setup vao, vbo

//render function 
//use program
//drawElements
//unuse program


// Step 2: Define Vertex Data
// Define an array of vertices where each vertex includes position and color data.

// c
// Copy code
// float vertices[] = {
//     // Positions         // Colors
//     -0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // Bottom left (Red)
//      0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // Bottom right (Green)
//      0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // Top (Blue)
// };
// Step 3: Create Vertex Array Object (VAO) and Vertex Buffer Object (VBO)
// Generate and bind a VAO and a VBO.

// Copy the vertex data into the VBO.

// c
// Copy code
// unsigned int VBO, VAO;
// glGenVertexArrays(1, &VAO);
// glGenBuffers(1, &VBO);

// glBindVertexArray(VAO);

// glBindBuffer(GL_ARRAY_BUFFER, VBO);
// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
// Step 4: Set Vertex Attribute Pointers
// Configure how OpenGL interprets the vertex data.

// c
// Copy code
// // Position attribute
// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
// glEnableVertexAttribArray(0);

// // Color attribute
// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
// glEnableVertexAttribArray(1);
// Step 5: Write Vertex and Fragment Shaders
// Create vertex and fragment shaders.

// Vertex Shader: Process vertex positions and pass color to the fragment shader.

// glsl
// Copy code
// #version 330 core
// layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec3 aColor;

// out vec3 ourColor; // Output color to fragment shader

// void main() {
//     gl_Position = vec4(aPos, 1.0);
//     ourColor = aColor;
// }
// Fragment Shader: Set the output color of the pixels.

// glsl
// Copy code
// #version 330 core
// out vec4 FragColor;
// in vec3 ourColor;

// void main() {
//     FragColor = vec4(ourColor, 1.0);
// }
// Step 6: Compile and Link Shaders
// Compile each shader and check for errors.
// Link the shaders into a shader program and activate it with glUseProgram.
// Step 7: Render Loop
// Clear the screen each frame.

// Bind the VAO.

// Draw the vertices using glDrawArrays.

// Swap the buffers and poll for window events.

// c
// Copy code
// while (!glfwWindowShouldClose(window)) {
//     // Input handling
//     // ...

//     // Rendering commands
//     glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//     glClear(GL_COLOR_BUFFER_BIT);

//     // Draw the triangle
//     glUseProgram(shaderProgram);
//     glBindVertexArray(VAO);
//     glDrawArrays(GL_TRIANGLES, 0, 3);

//     // Swap buffers and poll IO events
//     glfwSwapBuffers(window);
//     glfwPollEvents();
// }
// Step 8: Clean Up
// Delete the VAO, VBO, and shader program.

// Terminate GLFW.

// c
// Copy code
// glDeleteVertexArrays(1, &VAO);
// glDeleteBuffers(1, &VBO);
// glDeleteProgram(shaderProgram);

// glfwTerminate();