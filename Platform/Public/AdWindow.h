#ifndef AD_WINDOW_H
#define AD_WINDOW_H

#include "AdEngine.h"
#include "AdGraphicContext.h"
#include "Event/AdMouseEvent.h"

namespace ade{
    class AdWindow{
    public:
        AdWindow(const AdWindow&) = delete;
        AdWindow &operator=(const AdWindow&) = delete;
        virtual ~AdWindow() = default;

        static std::unique_ptr<AdWindow> Create(uint32_t width, uint32_t height, const char *title);

        virtual bool ShouldClose() = 0;
        virtual void PollEvents() = 0;
        virtual void SwapBuffer() = 0;
        virtual void* GetImplWindowPointer() const = 0;

        virtual void GetMousePos(glm::vec2 &mousePos) const = 0;
        virtual bool IsMouseDown(MouseButton mouseButton = MOUSE_BUTTON_LEFT) const = 0;
        virtual bool IsMouseUp(MouseButton mouseButton = MOUSE_BUTTON_LEFT) const = 0;
        virtual bool IsKeyDown(Key key) const = 0;
        virtual bool IsKeyUp(Key key) const = 0;
        virtual bool IsMinWindow() const = 0;
    protected:
        AdWindow() = default;
    };
}

#endif