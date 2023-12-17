#include <iostream>

#include<glad/glad.h>
#include <GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include "Core/DeltaTime.h"
#include "Input/MouseInput.h"
#include "Graphics/Camera.h"
#include "Graphics/Shader.h"

// Funcion declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);


// Global Variables
unsigned int WindowWidth = 1280;
unsigned int WindowHeight = 720;

bool ImGuiEditMode = true;

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

    glfwSwapBuffers(window);

    // Initialize Systems
    DeltaTime dt;

    MouseInput mouseInput(WindowWidth, WindowHeight, window);
    glfwSetWindowUserPointer(window, &mouseInput);
    glfwSetCursorPosCallback(window, MouseInput::MousePositionCallBack);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Camera camera(glm::vec3(0.0f, 2.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), &mouseInput);

    Shader* PBRLightShader = new Shader("Assets/Shaders/PBRLightVS.glsl", "Assets/Shaders/PBRLightFS.glsl");


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////// Game Loop                                                                                      
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    while (!glfwWindowShouldClose(window))
    {
        dt.CalculateDeltaTime();

        // Processes Inputs
        mouseInput.Update();
        if (ImGuiEditMode)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else
        {
            camera.ProcessInput(window, dt.GetDeltaTime());
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }


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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_I && action == GLFW_PRESS)
        ImGuiEditMode = !ImGuiEditMode;
}