#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window {
public:
    Window(int width, int height, const char* title);
    ~Window();

    bool ShouldClose() const;
    void PollEvents();
    void SwapBuffers();

private:
    GLFWwindow* m_window;
};