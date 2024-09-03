#ifndef ADWINDOWEVENT_H
#define ADWINDOWEVENT_H

#include "Event/AdEvent.h"

namespace ade{
    class AdFrameBufferResizeEvent : public AdEvent{
    public:
        AdFrameBufferResizeEvent(uint32_t width, uint32_t height) : mWidth(width), mHeight(height) {}
        [[nodiscard]] std::string ToString() const override {
            return AdEvent::ToString() + "( with=" + std::to_string(mWidth) + ", height=" + std::to_string(mHeight) + " )";
        }

        uint32_t mWidth, mHeight;
        EVENT_CLASS_TYPE(EVENT_TYPE_FRAME_BUFFER_RESIZE);
    };

    class AdWindowFocusEvent : public AdEvent{
    public:
        EVENT_CLASS_TYPE(EVENT_TYPE_WINDOW_FOCUS);
    };

    class AdWindowLostFocusEvent : public AdEvent{
    public:
        EVENT_CLASS_TYPE(EVENT_TYPE_WINDOW_LOST_FOCUS);
    };

    class AdWindowMovedEvent : public AdEvent{
    public:
        AdWindowMovedEvent(uint32_t xPos, uint32_t yPos) : mXPos(xPos), mYPos(yPos) {};
        [[nodiscard]] std::string ToString() const override {
            return AdEvent::ToString() + "( xPos=" + std::to_string(mXPos) + ", yPos=" + std::to_string(mYPos) + " )";
        }

        uint32_t mXPos;
        uint32_t mYPos;
        EVENT_CLASS_TYPE(EVENT_TYPE_WINDOW_MOVED);
    };

    class AdWindowCloseEvent : public AdEvent{
    public:
        EVENT_CLASS_TYPE(EVENT_TYPE_WINDOW_CLOSE);
    };
}

#endif