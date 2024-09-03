#ifndef ADKEYEVENT_H
#define ADKEYEVENT_H

#include "Event/AdEvent.h"

namespace ade{
    enum Key{
        KEY_UNKNOWN            = -1,

        /* Printable keys */
        KEY_SPACE              = 32,
        KEY_APOSTROPHE         = 39,  /* ' */
        KEY_COMMA              = 44,  /* , */
        KEY_MINUS              = 45,  /* - */
        KEY_PERIOD             = 46,  /* . */
        KEY_SLASH              = 47,  /* / */
        KEY_0                  = 48,
        KEY_1                  = 49,
        KEY_2                  = 50,
        KEY_3                  = 51,
        KEY_4                  = 52,
        KEY_5                  = 53,
        KEY_6                  = 54,
        KEY_7                  = 55,
        KEY_8                  = 56,
        KEY_9                  = 57,
        KEY_SEMICOLON          = 59,  /* , */
        KEY_EQUAL              = 61,  /* = */
        KEY_A                  = 65,
        KEY_B                  = 66,
        KEY_C                  = 67,
        KEY_D                  = 68,
        KEY_E                  = 69,
        KEY_F                  = 70,
        KEY_G                  = 71,
        KEY_H                  = 72,
        KEY_I                  = 73,
        KEY_J                  = 74,
        KEY_K                  = 75,
        KEY_L                  = 76,
        KEY_M                  = 77,
        KEY_N                  = 78,
        KEY_O                  = 79,
        KEY_P                  = 80,
        KEY_Q                  = 81,
        KEY_R                  = 82,
        KEY_S                  = 83,
        KEY_T                  = 84,
        KEY_U                  = 85,
        KEY_V                  = 86,
        KEY_W                  = 87,
        KEY_X                  = 88,
        KEY_Y                  = 89,
        KEY_Z                  = 90,
        KEY_LEFT_BRACKET       = 91,  /* [ */
        KEY_BACKSLASH          = 92,  /* \ */
        KEY_RIGHT_BRACKET      = 93,  /* ] */
        KEY_GRAVE_ACCENT       = 96,  /* ` */
        KEY_WORLD_1            = 161, /* non-US #1 */
        KEY_WORLD_2            = 162, /* non-US #2 */

        /* Function keys */
        KEY_ESCAPE             = 256,
        KEY_ENTER              = 257,
        KEY_TAB                = 258,
        KEY_BACKSPACE          = 259,
        KEY_INSERT             = 260,
        KEY_DELETE             = 261,
        KEY_RIGHT              = 262,
        KEY_LEFT               = 263,
        KEY_DOWN               = 264,
        KEY_UP                 = 265,
        KEY_PAGE_UP            = 266,
        KEY_PAGE_DOWN          = 267,
        KEY_HOME               = 268,
        KEY_END                = 269,
        KEY_CAPS_LOCK          = 280,
        KEY_SCROLL_LOCK        = 281,
        KEY_NUM_LOCK           = 282,
        KEY_PRINT_SCREEN       = 283,
        KEY_PAUSE              = 284,
        KEY_F1                 = 290,
        KEY_F2                 = 291,
        KEY_F3                 = 292,
        KEY_F4                 = 293,
        KEY_F5                 = 294,
        KEY_F6                 = 295,
        KEY_F7                 = 296,
        KEY_F8                 = 297,
        KEY_F9                 = 298,
        KEY_F10                = 299,
        KEY_F11                = 300,
        KEY_F12                = 301,
        KEY_F13                = 302,
        KEY_F14                = 303,
        KEY_F15                = 304,
        KEY_F16                = 305,
        KEY_F17                = 306,
        KEY_F18                = 307,
        KEY_F19                = 308,
        KEY_F20                = 309,
        KEY_F21                = 310,
        KEY_F22                = 311,
        KEY_F23                = 312,
        KEY_F24                = 313,
        KEY_F25                = 314,
        KEY_KP_0               = 320,
        KEY_KP_1               = 321,
        KEY_KP_2               = 322,
        KEY_KP_3               = 323,
        KEY_KP_4               = 324,
        KEY_KP_5               = 325,
        KEY_KP_6               = 326,
        KEY_KP_7               = 327,
        KEY_KP_8               = 328,
        KEY_KP_9               = 329,
        KEY_KP_DECIMAL         = 330,
        KEY_KP_DIVIDE          = 331,
        KEY_KP_MULTIPLY        = 332,
        KEY_KP_SUBTRACT        = 333,
        KEY_KP_ADD             = 334,
        KEY_KP_ENTER           = 335,
        KEY_KP_EQUAL           = 336,
        KEY_LEFT_SHIFT         = 340,
        KEY_LEFT_CONTROL       = 341,
        KEY_LEFT_ALT           = 342,
        KEY_LEFT_SUPER         = 343,
        KEY_RIGHT_SHIFT        = 344,
        KEY_RIGHT_CONTROL      = 345,
        KEY_RIGHT_ALT          = 346,
        KEY_RIGHT_SUPER        = 347,
        KEY_MENU               = 348,
        KEY_LAST               = KEY_MENU,
    };

    enum KeyMod{
        AD_MOD_SHIFT              = 0x0001,
        AD_MOD_CONTROL            = 0x0002,
        AD_MOD_ALT                = 0x0004,
        AD_MOD_SUPER              = 0x0008,
        AD_MOD_CAPS_LOCK          = 0x0010,
        AD_MOD_NUM_LOCK           = 0x0020,
    };

    static const char* AdKeyToStr(Key keyCode){
        switch (keyCode) {
            ENUM_TO_STR(KEY_UNKNOWN);
            ENUM_TO_STR(KEY_SPACE);
            ENUM_TO_STR(KEY_APOSTROPHE);
            ENUM_TO_STR(KEY_COMMA);
            ENUM_TO_STR(KEY_MINUS);
            ENUM_TO_STR(KEY_PERIOD);
            ENUM_TO_STR(KEY_SLASH);
            ENUM_TO_STR(KEY_0);
            ENUM_TO_STR(KEY_1);
            ENUM_TO_STR(KEY_2);
            ENUM_TO_STR(KEY_3);
            ENUM_TO_STR(KEY_4);
            ENUM_TO_STR(KEY_5);
            ENUM_TO_STR(KEY_6);
            ENUM_TO_STR(KEY_7);
            ENUM_TO_STR(KEY_8);
            ENUM_TO_STR(KEY_9);
            ENUM_TO_STR(KEY_SEMICOLON);
            ENUM_TO_STR(KEY_EQUAL);
            ENUM_TO_STR(KEY_A);
            ENUM_TO_STR(KEY_B);
            ENUM_TO_STR(KEY_C);
            ENUM_TO_STR(KEY_D);
            ENUM_TO_STR(KEY_E);
            ENUM_TO_STR(KEY_F);
            ENUM_TO_STR(KEY_G);
            ENUM_TO_STR(KEY_H);
            ENUM_TO_STR(KEY_I);
            ENUM_TO_STR(KEY_J);
            ENUM_TO_STR(KEY_K);
            ENUM_TO_STR(KEY_L);
            ENUM_TO_STR(KEY_M);
            ENUM_TO_STR(KEY_N);
            ENUM_TO_STR(KEY_O);
            ENUM_TO_STR(KEY_P);
            ENUM_TO_STR(KEY_Q);
            ENUM_TO_STR(KEY_R);
            ENUM_TO_STR(KEY_S);
            ENUM_TO_STR(KEY_T);
            ENUM_TO_STR(KEY_U);
            ENUM_TO_STR(KEY_V);
            ENUM_TO_STR(KEY_W);
            ENUM_TO_STR(KEY_X);
            ENUM_TO_STR(KEY_Y);
            ENUM_TO_STR(KEY_Z);
            ENUM_TO_STR(KEY_LEFT_BRACKET);
            ENUM_TO_STR(KEY_BACKSLASH);
            ENUM_TO_STR(KEY_RIGHT_BRACKET);
            ENUM_TO_STR(KEY_GRAVE_ACCENT);
            ENUM_TO_STR(KEY_WORLD_1);
            ENUM_TO_STR(KEY_WORLD_2);
            ENUM_TO_STR(KEY_ESCAPE);
            ENUM_TO_STR(KEY_ENTER);
            ENUM_TO_STR(KEY_TAB);
            ENUM_TO_STR(KEY_BACKSPACE);
            ENUM_TO_STR(KEY_INSERT);
            ENUM_TO_STR(KEY_DELETE);
            ENUM_TO_STR(KEY_RIGHT);
            ENUM_TO_STR(KEY_LEFT);
            ENUM_TO_STR(KEY_DOWN);
            ENUM_TO_STR(KEY_UP);
            ENUM_TO_STR(KEY_PAGE_UP);
            ENUM_TO_STR(KEY_PAGE_DOWN);
            ENUM_TO_STR(KEY_HOME);
            ENUM_TO_STR(KEY_END);
            ENUM_TO_STR(KEY_CAPS_LOCK);
            ENUM_TO_STR(KEY_SCROLL_LOCK);
            ENUM_TO_STR(KEY_NUM_LOCK);
            ENUM_TO_STR(KEY_PRINT_SCREEN);
            ENUM_TO_STR(KEY_PAUSE);
            ENUM_TO_STR(KEY_F1);
            ENUM_TO_STR(KEY_F2);
            ENUM_TO_STR(KEY_F3);
            ENUM_TO_STR(KEY_F4);
            ENUM_TO_STR(KEY_F5);
            ENUM_TO_STR(KEY_F6);
            ENUM_TO_STR(KEY_F7);
            ENUM_TO_STR(KEY_F8);
            ENUM_TO_STR(KEY_F9);
            ENUM_TO_STR(KEY_F10);
            ENUM_TO_STR(KEY_F11);
            ENUM_TO_STR(KEY_F12);
            ENUM_TO_STR(KEY_F13);
            ENUM_TO_STR(KEY_F14);
            ENUM_TO_STR(KEY_F15);
            ENUM_TO_STR(KEY_F16);
            ENUM_TO_STR(KEY_F17);
            ENUM_TO_STR(KEY_F18);
            ENUM_TO_STR(KEY_F19);
            ENUM_TO_STR(KEY_F20);
            ENUM_TO_STR(KEY_F21);
            ENUM_TO_STR(KEY_F22);
            ENUM_TO_STR(KEY_F23);
            ENUM_TO_STR(KEY_F24);
            ENUM_TO_STR(KEY_F25);
            ENUM_TO_STR(KEY_KP_0);
            ENUM_TO_STR(KEY_KP_1);
            ENUM_TO_STR(KEY_KP_2);
            ENUM_TO_STR(KEY_KP_3);
            ENUM_TO_STR(KEY_KP_4);
            ENUM_TO_STR(KEY_KP_5);
            ENUM_TO_STR(KEY_KP_6);
            ENUM_TO_STR(KEY_KP_7);
            ENUM_TO_STR(KEY_KP_8);
            ENUM_TO_STR(KEY_KP_9);
            ENUM_TO_STR(KEY_KP_DECIMAL);
            ENUM_TO_STR(KEY_KP_DIVIDE);
            ENUM_TO_STR(KEY_KP_MULTIPLY);
            ENUM_TO_STR(KEY_KP_SUBTRACT);
            ENUM_TO_STR(KEY_KP_ADD);
            ENUM_TO_STR(KEY_KP_ENTER);
            ENUM_TO_STR(KEY_KP_EQUAL);
            ENUM_TO_STR(KEY_LEFT_SHIFT);
            ENUM_TO_STR(KEY_LEFT_CONTROL);
            ENUM_TO_STR(KEY_LEFT_ALT);
            ENUM_TO_STR(KEY_LEFT_SUPER);
            ENUM_TO_STR(KEY_RIGHT_SHIFT);
            ENUM_TO_STR(KEY_RIGHT_CONTROL);
            ENUM_TO_STR(KEY_RIGHT_ALT);
            ENUM_TO_STR(KEY_RIGHT_SUPER);
            ENUM_TO_STR(KEY_MENU);
        }
        return "unknown";
    }

    static const char* AdKeyModToStr(KeyMod keyMod){
        switch (keyMod) {
            ENUM_TO_STR(AD_MOD_SHIFT);
            ENUM_TO_STR(AD_MOD_CONTROL);
            ENUM_TO_STR(AD_MOD_ALT);
            ENUM_TO_STR(AD_MOD_SUPER);
            ENUM_TO_STR(AD_MOD_CAPS_LOCK);
            ENUM_TO_STR(AD_MOD_NUM_LOCK);
        }
        return "unknown";
    }

    class AdKeyPressEvent : public AdEvent{
    public:
        AdKeyPressEvent(Key keyCode, KeyMod keyMod, bool repeat) : mKeyCode(keyCode), mKeyMod(keyMod), mRepeat(repeat){

        };

        [[nodiscard]] std::string ToString() const override {
            std::stringstream ss;
            ss << AdEvent::ToString();
            ss << "( keyCode=" << AdKeyToStr(mKeyCode);
            ss << ", keyMod=" << AdKeyModToStr(mKeyMod);
            ss << ", isRepeat=" << mRepeat << " )";
            return ss.str();
        }

        [[nodiscard]] bool IsShiftPressed() const { return mKeyMod & AD_MOD_SHIFT; };
        [[nodiscard]] bool IsControlPressed() const { return mKeyMod & AD_MOD_CONTROL; };
        [[nodiscard]] bool IsAltPressed() const { return mKeyMod & AD_MOD_ALT; };
        [[nodiscard]] bool IsSuperPressed() const { return mKeyMod & AD_MOD_SUPER; };
        [[nodiscard]] bool IsCapsLockPressed() const { return mKeyMod & AD_MOD_CAPS_LOCK; };
        [[nodiscard]] bool IsNumLockPressed() const { return mKeyMod & AD_MOD_NUM_LOCK; };
        [[nodiscard]] bool IsRepeat() const { return mRepeat; };

        Key mKeyCode;
        KeyMod mKeyMod;
        bool mRepeat;
        EVENT_CLASS_TYPE(EVENT_TYPE_KEY_PRESS);
    };

    class AdKeyReleaseEvent : public AdEvent{
    public:
        AdKeyReleaseEvent(Key keyCode) : mKeyCode(keyCode){

        };
        [[nodiscard]] std::string ToString() const override {
            std::stringstream ss;
            ss << AdEvent::ToString();
            ss << "( keyCode=" << AdKeyToStr(mKeyCode) << " )";
            return ss.str();
        }

        Key mKeyCode;
        EVENT_CLASS_TYPE(EVENT_TYPE_KEY_RELEASE);
    };
}

#endif