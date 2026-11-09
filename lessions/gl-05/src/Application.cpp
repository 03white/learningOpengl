// glad 必须在 glfw 之前 include
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "Application.h"

#include <iostream>

Application& Application::instance() {
    static Application app;  // C++11 起局部静态变量初始化线程安全
    return app;
}

Application::~Application() {
    if (initialized_) {
        glfwTerminate();
    }
}

bool Application::init(int width, int height, const std::string& title) {
    if (initialized_) {
        std::cerr << "Application already initialized" << std::endl;
        return true;
    }

    width_ = width;
    height_ = height;

    // 1. 初始化 GLFW，指定 OpenGL 3.3 Core Profile
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 2. 创建窗口
    window_ = glfwCreateWindow(width_, height_, title.c_str(), nullptr, nullptr);
    if (window_ == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window_);

    // 把 this 存到窗口，回调里取回来
    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, framebufferSizeCallback);

    // 3. glad 加载 OpenGL 函数指针
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        window_ = nullptr;
        return false;
    }

    glViewport(0, 0, width_, height_);
    initialized_ = true;
    return true;
}

void Application::run() {
    if (!initialized_) {
        std::cerr << "Application::run() called before init()" << std::endl;
        return;
    }

    while (!glfwWindowShouldClose(window_)) {
        processInput();

        glClearColor(clearColor_[0], clearColor_[1], clearColor_[2], clearColor_[3]);
        glClear(GL_COLOR_BUFFER_BIT);

        if (renderCallback_) {
            renderCallback_();
        }

        glfwSwapBuffers(window_);
        glfwPollEvents();
    }
}

void Application::setClearColor(float r, float g, float b, float a) {
    clearColor_[0] = r;
    clearColor_[1] = g;
    clearColor_[2] = b;
    clearColor_[3] = a;
}

void Application::processInput() {
    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, true);
    }
}

void Application::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    if (auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window))) {
        app->width_ = width;
        app->height_ = height;
    }
}
