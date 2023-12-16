#include <iostream>

#include<glad/glad.h>
#include <GLFW/glfw3.h>

#include "Core/DeltaTime.h"

// Funcion declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);


// Global Variables
unsigned int WindowWidth = 1280;
unsigned int WindowHeight = 720;

int main(void)
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //// Initializations                                                                                        
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    const char* glsl_version = "#version 460";
    // GLFW Version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    // GLFW Core Profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, "ZoEngine Window", NULL, NULL);
    // Error Check for Window
    if (window == NULL)
    {
        std::cout << "Failed to create Window" << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Load GLAD
    gladLoadGL();

    // Set Viewport
    glViewport(0, 0, WindowWidth, WindowHeight);


    // Initialize Systems
    DeltaTime dt;



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////// Game Loop                                                                                      
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    while (!glfwWindowShouldClose(window))
    {
        dt.CalculateDeltaTime();

        // Specify color of background
        glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////// Clean Up                                                                                   
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    glfwTerminate();
    return 0;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    WindowWidth = width;
    WindowHeight = height;
    glViewport(0, 0, WindowWidth, WindowHeight);
}