#include "Window.h"
#include <stdexcept>


Window::Window(int width, int height, const char* title) {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create window");
    }

    glfwMakeContextCurrent(m_window);

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        throw std::runtime_error("Failed to initialize GLEW");
    }
}

Window::~Window() {
    glfwTerminate();
}

bool Window::ShouldClose() const {
    return glfwWindowShouldClose(m_window);
}

void Window::PollEvents() {
    glfwPollEvents();
}

void Window::SwapBuffers() {
    glfwSwapBuffers(m_window);
}