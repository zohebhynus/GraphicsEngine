#include "Application.h"

int WindowHeight;
int WindowWidth;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int Application::Initialize()
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    const char* glsl_version = "#version 460";
    // GLFW Version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

    // GLFW Core Profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    WindowHeight = 720;
    WindowWidth = 1280;

    /* Create a windowed mode window and its OpenGL context */
    m_Window = glfwCreateWindow(WindowWidth, WindowHeight, "ZoEngine Window", NULL, NULL);
    if (!m_Window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(m_Window);
    glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback);

    // Load GLAD
    gladLoadGL();

    // Set Viewport
    glViewport(0, 0, WindowWidth, WindowHeight);

    return 0;
}

void Application::Run()
{
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(m_Window))
    {
        dt.CalculateDeltaTime();

        // Specify color of background
        glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(m_Window);

        /* Poll for and process events */
        glfwPollEvents();
    }
}

void Application::Release()
{
    glfwTerminate();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    WindowWidth = width;
    WindowHeight = height;
    glViewport(0, 0, WindowWidth, WindowHeight);
}
