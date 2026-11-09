#pragma once

#include <functional>
#include <string>

// 前向声明，避免在头文件里暴露 glfw
struct GLFWwindow;

// 窗口 / OpenGL 上下文管理单例
// 用法：
//   auto& app = Application::instance();
//   app.init(800, 600, "LearnOpenGL");
//   app.setRenderCallback([]{ /* 每帧绘制 */ });
//   app.run();
class Application {
public:
    // 每帧渲染回调（在清屏之后、交换缓冲之前调用）
    using RenderCallback = std::function<void()>;

    // 获取单例
    static Application& instance();

    // 初始化 GLFW + 窗口 + glad，成功返回 true
    bool init(int width = 800, int height = 600,
              const std::string& title = "LearnOpenGL");

    // 进入渲染主循环（阻塞直到窗口关闭）
    void run();

    // 设置每帧渲染回调
    void setRenderCallback(RenderCallback cb) { renderCallback_ = std::move(cb); }

    // 设置清屏颜色
    void setClearColor(float r, float g, float b, float a = 1.0f);

    // 访问底层窗口指针（需要直接用 glfw API 时）
    GLFWwindow* window() const { return window_; }
    int width() const { return width_; }
    int height() const { return height_; }

    // 禁止拷贝 / 移动（单例）
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

private:
    Application() = default;
    ~Application();

    // 默认输入处理：ESC 关闭窗口
    void processInput();

    // GLFW 帧缓冲尺寸回调
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    GLFWwindow* window_ = nullptr;
    int width_ = 800;
    int height_ = 600;
    float clearColor_[4] = {0.1f, 0.3f, 0.3f, 1.0f};
    bool initialized_ = false;
    RenderCallback renderCallback_;
};
