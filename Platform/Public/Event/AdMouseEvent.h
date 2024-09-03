#ifndef ADMOUSEEVENT_H
#define ADMOUSEEVENT_H

#include "Event/AdKeyEvent.h"

namespace ade{
    enum MouseButton{
        MOUSE_BUTTON_LEFT   =   0,
        MOUSE_BUTTON_RIGHT  =   1,
        MOUSE_BUTTON_MIDDLE =   2,
    };

    static const char* AdMouseButtonToStr(MouseButton mouseButton){
        switch (mouseButton) {
            ENUM_TO_STR(MOUSE_BUTTON_LEFT);
            ENUM_TO_STR(MOUSE_BUTTON_RIGHT);
            ENUM_TO_STR(MOUSE_BUTTON_MIDDLE);
        }
        return "unknown";
    }

    class AdMouseButtonPressEvent : public AdEvent{
    public:
        AdMouseButtonPressEvent(MouseButton mouseButton, KeyMod keyMod, bool repeat) : mMouseButton(mouseButton), mKeyMod(keyMod), mRepeat(repeat) {
        };

        [[nodiscard]] std::string ToString() const override {
            std::stringstream ss;
            ss << AdEvent::ToString();
            ss << "( mouseButton=" << AdMouseButtonToStr(mMouseButton);
            ss << ", keyMod=" << AdKeyModToStr(mKeyMod);
            ss << ", isRepeat=" << mRepeat << " )";
            return ss.str();
        }

        [[nodiscard]] bool IsShiftPressed() const { return mKeyMod & AD_MOD_SHIFT; };
        [[nodiscard]] bool IsControlPressed() const { return mKeyMod & AD_MOD_CONTROL; };
        [[nodiscard]] bool IsAltPressed() const { return mKeyMod & AD_MOD_ALT; };
        [[nodiscard]] bool IsSuperPressed() const { return mKeyMod & AD_MOD_SUPER; };
        [[nodiscard]] bool IsCapsLockPressed() const { return mKeyMod & AD_MOD_CAPS_LOCK; };    //TODO mod do not return this.
        [[nodiscard]] bool IsNumLockPressed() const { return mKeyMod & AD_MOD_NUM_LOCK; };      //TODO mod do not return this.
        [[nodiscard]] bool IsRepeat() const { return mRepeat; };

        MouseButton mMouseButton;
        KeyMod mKeyMod;
        bool mRepeat;
        EVENT_CLASS_TYPE(EVENT_TYPE_MOUSE_PRESS);
    };

    class AdMouseButtonReleaseEvent : public AdEvent{
    public:
        AdMouseButtonReleaseEvent(MouseButton mouseButton) : mMouseButton(mouseButton){

        };
        [[nodiscard]] std::string ToString() const override {
            std::stringstream ss;
            ss << AdEvent::ToString();
            ss << "( mouseButton=" << AdMouseButtonToStr(mMouseButton) << " )";
            return ss.str();
        }

        MouseButton mMouseButton;
        EVENT_CLASS_TYPE(EVENT_TYPE_MOUSE_RELEASE);
    };

    class AdMouseMovedEvent : public AdEvent{
    public:
        AdMouseMovedEvent(float xPos, float yPos) : mXPos(xPos), mYPos(yPos) {

        }
        [[nodiscard]] std::string ToString() const override {
            std::stringstream ss;
            ss << AdEvent::ToString();
            ss << "( xPos=" << mXPos;
            ss << ", yPos=" << mYPos << " )";
            return ss.str();
        }

        float mXPos;
        float mYPos;
        EVENT_CLASS_TYPE(EVENT_TYPE_MOUSE_MOVED);
    };

    class AdMouseScrollEvent : public AdEvent{
    public:
        AdMouseScrollEvent(float xOffset, float yOffset) : mXOffset(xOffset), mYOffset(yOffset) {};
        [[nodiscard]] std::string ToString() const override {
            std::stringstream ss;
            ss << AdEvent::ToString();
            ss << "( xOffset=" << mXOffset;
            ss << ", yOffset=" << mYOffset << " )";
            return ss.str();
        }

        float mXOffset;
        float mYOffset;
        EVENT_CLASS_TYPE(EVENT_TYPE_MOUSE_SCROLLED);
    };
}

#endif