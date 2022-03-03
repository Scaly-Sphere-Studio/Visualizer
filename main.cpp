#include <iostream>
// This header includes ALL the headers from the SSS/GL library
#include <SSS/GL.hpp>
#include "shader.hpp"
#include "visualizer.h"

int main(void)
{
    /* Initialize the library */
    if (!glfwInit()) {
        return -1;
    }
        
    // Create a window. This is where the OpenGL context is created.
    GLFWwindow* window;

    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        __LOG_ERR("GLFW couldn't create the window context");
    }

    glfwMakeContextCurrent(window);
    /*glfwSetWindowPos(window, 100, 100);*/
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }
   
    //// Set the window's callbacks
    //window->setCallback(glfwSetKeyCallback, key_callback);
    //window->setCallback(glfwSetMouseButtonCallback, mouse_button_callback);

    //// Set the VSYNC
    //window->setVSYNC(false);

    // Enable blending (transparency)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //GL TRIANGLE

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    static const GLfloat g_vertex_buffer_data[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.5f,  0.5f, 0.0f,
    
    0.5f,  0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f,
    -0.5f,  -0.5f, 0.0f,
    };


    Box box{ 0.5f , 0.5f };

    // This will identify our vertex buffer
    GLuint vertexbuffer;
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, box.model.size() * sizeof(Vertex), &box.model, GL_STATIC_DRAW);

    GLuint programID = LoadShaders("triangle.vert", "triangle.frag");
    glUseProgram(programID);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll everything (events, texture threads, text areas threads...)
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0, 0, 1, 0);

        



       






        //Render
        //// 1st attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );
        // Draw the triangle !
        
        glDrawArrays(GL_TRIANGLES, 0, box.model.size()); // Starting from vertex 0; 3 vertices total -> 1 triangle
        glDisableVertexAttribArray(0);
        
        
    }
}