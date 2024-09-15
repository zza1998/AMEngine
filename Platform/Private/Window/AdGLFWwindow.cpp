#include "Window/AdGLFWwindow.h"
#include "AdLog.h"
#include "GLFW/glfw3native.h"
#include "Event/AdEventDispatcher.h"

namespace ade{
    AdGLFWwindow::AdGLFWwindow(uint32_t width, uint32_t height, const char *title) {
        if(!glfwInit()){
            LOG_E("Failed to init glfw.");
            return;
        }
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_VISIBLE, GL_FALSE);

        mGLFWwindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
        if(!mGLFWwindow){
            LOG_E("Failed to create glfw window.");
            return;
        }

        GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
        if(primaryMonitor){
            int xPos, yPos, workWidth, workHeight;
            glfwGetMonitorWorkarea(primaryMonitor, &xPos, &yPos, &workWidth, &workHeight);
            glfwSetWindowPos(mGLFWwindow, workWidth / 2 - width / 2, workHeight / 2 - height / 2);
        }

        // For OpenGL/OpenGLES https://www.glfw.org/docs/latest/group__context.html#ga6d4e0cdf151b5e579bd67f13202994ed
        // This function sets the swap interval for the current OpenGL or OpenGL ES context,
        // i.e. the number of screen updates to wait from the time glfwSwapBuffers was called before swapping the buffers and returning.
        //glfwSwapInterval(0);

        glfwMakeContextCurrent(mGLFWwindow);

        SetupWindowCallbacks();

        // show window
        glfwShowWindow(mGLFWwindow);
    }

    AdGLFWwindow::~AdGLFWwindow() {
        glfwDestroyWindow(mGLFWwindow);
        glfwTerminate();
        LOG_I("The application running end.");
    }

    bool AdGLFWwindow::ShouldClose() {
        return glfwWindowShouldClose(mGLFWwindow);
    }

    void AdGLFWwindow::PollEvents() {
        glfwPollEvents();
    }

    void AdGLFWwindow::SwapBuffer() {
        glfwSwapBuffers(mGLFWwindow);
    }

    void AdGLFWwindow::GetMousePos(glm::vec2 &mousePos) const {
        double x, y;
        glfwGetCursorPos(mGLFWwindow, &x, &y);
        mousePos = { x, y };
    }

    bool AdGLFWwindow::IsMouseDown(MouseButton mouseButton) const {
        return glfwGetMouseButton(mGLFWwindow, mouseButton) == GLFW_PRESS;
    }

    bool AdGLFWwindow::IsMouseUp(MouseButton mouseButton) const {
        return glfwGetMouseButton(mGLFWwindow, mouseButton) == GLFW_RELEASE;
    }

    bool AdGLFWwindow::IsKeyDown(Key key) const {
        return glfwGetKey(mGLFWwindow, key) == GLFW_PRESS;
    }

    bool AdGLFWwindow::IsKeyUp(Key key) const {
        return glfwGetKey(mGLFWwindow, key) == GLFW_RELEASE;
    }

    bool AdGLFWwindow::IsMinWindow() const {
        return glfwGetWindowAttrib(mGLFWwindow, GLFW_ICONIFIED) != 0;
    }
    void AdGLFWwindow::SetupWindowCallbacks() {
        glfwSetWindowUserPointer(mGLFWwindow, this);
        glfwSetFramebufferSizeCallback(mGLFWwindow, [](GLFWwindow* window, int width, int height){
            auto *adWindow = static_cast<AdGLFWwindow*>(glfwGetWindowUserPointer(window));
            if(adWindow){
                AdFrameBufferResizeEvent fbResizeEvent{ static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
                AdEventDispatcher::GetInstance()->Dispatch(fbResizeEvent);
            }
        });

        glfwSetWindowFocusCallback(mGLFWwindow, [](GLFWwindow* window, int focused){
            auto *adWindow = static_cast<AdGLFWwindow *>(glfwGetWindowUserPointer(window));
            if(adWindow){
                if(focused){
                    AdWindowFocusEvent windowFocusEvent{};
                    AdEventDispatcher::GetInstance()->Dispatch(windowFocusEvent);
                } else {
                    AdWindowLostFocusEvent windowLostFocusEvent{};
                    AdEventDispatcher::GetInstance()->Dispatch(windowLostFocusEvent);
                }
            }
        });

        glfwSetWindowPosCallback(mGLFWwindow, [](GLFWwindow* window, int xpos, int ypos){
            auto *adWindow = static_cast<AdGLFWwindow *>(glfwGetWindowUserPointer(window));
            if(adWindow){
                AdWindowMovedEvent windowMovedEvent{ static_cast<uint32_t>(xpos), static_cast<uint32_t>(ypos) };
                AdEventDispatcher::GetInstance()->Dispatch(windowMovedEvent);
            }
        });

        glfwSetWindowCloseCallback(mGLFWwindow, [](GLFWwindow* window){
            auto *adWindow = static_cast<AdGLFWwindow *>(glfwGetWindowUserPointer(window));
            if(adWindow){
                AdWindowCloseEvent windowCloseEvent{};
                AdEventDispatcher::GetInstance()->Dispatch(windowCloseEvent);
            }
        });

        glfwSetKeyCallback(mGLFWwindow, [](GLFWwindow* window, int key, int scancode, int action, int mods){
            auto *adWindow = static_cast<AdGLFWwindow *>(glfwGetWindowUserPointer(window));
            if(adWindow){
                if(action == GLFW_RELEASE){
                    AdKeyReleaseEvent keyReleaseEvent{static_cast<Key>(key) };
                    AdEventDispatcher::GetInstance()->Dispatch(keyReleaseEvent);
                } else {
                    AdKeyPressEvent keyPressEvent{static_cast<Key>(key), static_cast<KeyMod>(mods), action == GLFW_REPEAT };
                    AdEventDispatcher::GetInstance()->Dispatch(keyPressEvent);
                }
            }
        });

        glfwSetMouseButtonCallback(mGLFWwindow, [](GLFWwindow* window, int button, int action, int mods){
            auto *adWindow = static_cast<AdGLFWwindow *>(glfwGetWindowUserPointer(window));
            if(adWindow){
                if(action == GLFW_PRESS){
                    AdMouseButtonPressEvent mouseButtonPressEvent{static_cast<MouseButton>(button), static_cast<KeyMod>(mods), false };
                    AdEventDispatcher::GetInstance()->Dispatch(mouseButtonPressEvent);
                }
                if(action == GLFW_RELEASE){
                    AdMouseButtonReleaseEvent mouseButtonReleaseEvent{static_cast<MouseButton>(button) };
                    AdEventDispatcher::GetInstance()->Dispatch(mouseButtonReleaseEvent);
                }
            }
        });

        glfwSetCursorPosCallback(mGLFWwindow, [](GLFWwindow* window, double xpos, double ypos){
            auto *adWindow = static_cast<AdGLFWwindow *>(glfwGetWindowUserPointer(window));
            if(adWindow){
                AdMouseMovedEvent mouseMovedEvent{ static_cast<float>(xpos), static_cast<float>(ypos) };
                AdEventDispatcher::GetInstance()->Dispatch(mouseMovedEvent);
            }
        });

        glfwSetScrollCallback(mGLFWwindow, [](GLFWwindow* window, double xoffset, double yoffset){
            auto *adWindow = static_cast<AdGLFWwindow *>(glfwGetWindowUserPointer(window));
            if(adWindow){
                AdMouseScrollEvent mouseScrollEvent{ static_cast<float>(xoffset), static_cast<float>(yoffset) };
                AdEventDispatcher::GetInstance()->Dispatch(mouseScrollEvent);
            }
        });
    }
}