#ifndef AD_GLFWWINDOW_H
#define AD_GLFWWINDOW_H

#include "AdWindow.h"
#include "GLFW/glfw3.h"

namespace ade{
    class AdGLFWwindow : public AdWindow{
    public:
        AdGLFWwindow() = delete;
        AdGLFWwindow(uint32_t width, uint32_t height, const char *title);
        ~AdGLFWwindow() override;

        bool ShouldClose() override;
        void PollEvents() override;
        void SwapBuffer() override;

        void* GetImplWindowPointer() const override { return mGLFWwindow; };
        void GetMousePos(glm::vec2 &mousePos) const override;
        bool IsMouseDown(MouseButton mouseButton) const override;
        bool IsMouseUp(MouseButton mouseButton) const override;
        bool IsKeyDown(Key key) const override;
        bool IsKeyUp(Key key) const override;
    private:
        void SetupWindowCallbacks();

        GLFWwindow *mGLFWwindow;
    };
}

#endif