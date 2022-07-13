#pragma once

#include <cstdint>
#include <unordered_map>

#include <render/keys_mapping.hpp>
#include <math/vec2d.hpp>

namespace arti::render {

    class app;

    class input_manager {

        friend class app;

        struct input_state {
            bool bPressed = false;
            bool bReleased = false;
            bool bHeld = false;
        };

    public:
        input_manager();
        ~input_manager();

        void update();

        bool keyHeld(key_t keyId) const;
        bool keyPressed(key_t keyId) const;
        bool keyReleased(key_t keyId) const;

        bool buttonHeld(button_t buttonId) const;
        bool buttonPressed(button_t buttonId) const;
        bool buttonReleased(button_t buttonId) const;

        int32_t verticalScroll() const;
        int32_t horizontalScroll() const;

        math::vec2di mousePos() const;

        int32_t mouseX() const;
        int32_t mouseY() const;

    private:
        void keyPress(const int32_t &keyId);
        void keyRelease(const int32_t &keyId);

        void buttonPress(const Uint8 &buttonId);
        void buttonRelease(const Uint8 &buttonId);

        void verticalScroll(const int32_t &val);
        void horizontalScroll(const int32_t &val);
        void mouseCoords(const math::vec2di &coords);

        std::unordered_map<key_t, bool> m_actKeyMap;
        std::unordered_map<key_t, bool> m_prevKeyMap;
        std::unordered_map<key_t, input_state> m_keyStates;

        std::unordered_map<button_t, bool> m_actButtonMap;
        std::unordered_map<button_t, bool> m_prevButtonMap;
        std::unordered_map<button_t, input_state> m_buttonStates;

        int32_t m_verticalScroll;
        int32_t m_horizontalScroll;
        bool m_scrollUpdated;
        math::vec2di m_mouseCords;
    };

}