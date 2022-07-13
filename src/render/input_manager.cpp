#include <render/input_manager.hpp>

#include <SDL2/SDL_mouse.h>

namespace arti::render {

    input_manager::input_manager()
      : m_mouseCords(0, 0), 
        m_verticalScroll(0), 
        m_horizontalScroll(0),
        m_scrollUpdated(false) {
        m_actKeyMap = {
            { key_t::Unknown           , false },  { key_t::F11               , false },  { key_t::Kp_Leftparen      , false },
            { key_t::Return            , false },  { key_t::F12               , false },  { key_t::Kp_Rightparen     , false },
            { key_t::Escape            , false },  { key_t::Printscreen       , false },  { key_t::Kp_Leftbrace      , false },
            { key_t::Backspace         , false },  { key_t::Scrolllock        , false },  { key_t::Kp_Rightbrace     , false },
            { key_t::Tab               , false },  { key_t::Pause             , false },  { key_t::Kp_Tab            , false },
            { key_t::Space             , false },  { key_t::Insert            , false },  { key_t::Kp_Backspace      , false },
            { key_t::Exclaim           , false },  { key_t::Home              , false },  { key_t::Kp_A              , false },
            { key_t::Quotedbl          , false },  { key_t::Pageup            , false },  { key_t::Kp_B              , false },
            { key_t::Hash              , false },  { key_t::Kdelete           , false },  { key_t::Kp_C              , false },
            { key_t::Percent           , false },  { key_t::End               , false },  { key_t::Kp_D              , false },
            { key_t::Dollar            , false },  { key_t::Pagedown          , false },  { key_t::Kp_E              , false },
            { key_t::Ampersand         , false },  { key_t::Right             , false },  { key_t::Kp_F              , false },
            { key_t::Quote             , false },  { key_t::Left              , false },  { key_t::Kp_Xor            , false },
            { key_t::Leftparen         , false },  { key_t::Down              , false },  { key_t::Kp_Power          , false },
            { key_t::Rightparen        , false },  { key_t::Up                , false },  { key_t::Kp_Percent        , false },
            { key_t::Asterisk          , false },  { key_t::Numlockclear      , false },  { key_t::Kp_Less           , false },
            { key_t::Plus              , false },  { key_t::Kp_Divide         , false },  { key_t::Kp_Greater        , false },
            { key_t::Comma             , false },  { key_t::Kp_Multiply       , false },  { key_t::Kp_Ampersand      , false },
            { key_t::Minus             , false },  { key_t::Kp_Minus          , false },  { key_t::Kp_Dblampersand   , false },
            { key_t::Period            , false },  { key_t::Kp_Plus           , false },  { key_t::Kp_Verticalbar    , false },
            { key_t::Slash             , false },  { key_t::Kp_Enter          , false },  { key_t::Kp_Dblverticalbar , false },
            { key_t::K0                , false },  { key_t::Kp_1              , false },  { key_t::Kp_Colon          , false },
            { key_t::K1                , false },  { key_t::Kp_2              , false },  { key_t::Kp_Hash           , false },
            { key_t::K2                , false },  { key_t::Kp_3              , false },  { key_t::Kp_Space          , false },
            { key_t::K3                , false },  { key_t::Kp_4              , false },  { key_t::Kp_At             , false },
            { key_t::K4                , false },  { key_t::Kp_5              , false },  { key_t::Kp_Exclam         , false },
            { key_t::K5                , false },  { key_t::Kp_6              , false },  { key_t::Kp_Memstore       , false },
            { key_t::K6                , false },  { key_t::Kp_7              , false },  { key_t::Kp_Memrecall      , false },
            { key_t::K7                , false },  { key_t::Kp_8              , false },  { key_t::Kp_Memclear       , false },
            { key_t::K8                , false },  { key_t::Kp_9              , false },  { key_t::Kp_Memadd         , false },
            { key_t::K9                , false },  { key_t::Kp_0              , false },  { key_t::Kp_Memsubtract    , false },
            { key_t::Colon             , false },  { key_t::Kp_Period         , false },  { key_t::Kp_Memmultiply    , false },
            { key_t::Semicolon         , false },  { key_t::Application       , false },  { key_t::Kp_Memdivide      , false },
            { key_t::Less              , false },  { key_t::Power             , false },  { key_t::Kp_Plusminus      , false },
            { key_t::Equals            , false },  { key_t::Kp_Equals         , false },  { key_t::Kp_Clear          , false },
            { key_t::Greater           , false },  { key_t::F13               , false },  { key_t::Kp_Clearentry     , false },
            { key_t::Question          , false },  { key_t::F14               , false },  { key_t::Kp_Binary         , false },
            { key_t::At                , false },  { key_t::F15               , false },  { key_t::Kp_Octal          , false },
            { key_t::Leftbracket       , false },  { key_t::F16               , false },  { key_t::Kp_Decimal        , false },
            { key_t::Backslash         , false },  { key_t::F17               , false },  { key_t::Kp_Hexadecimal    , false },
            { key_t::Rightbracket      , false },  { key_t::F18               , false },  { key_t::Lctrl             , false },
            { key_t::Caret             , false },  { key_t::F19               , false },  { key_t::Lshift            , false },
            { key_t::Underscore        , false },  { key_t::F20               , false },  { key_t::Lalt              , false },
            { key_t::Backquote         , false },  { key_t::F21               , false },  { key_t::Lgui              , false },
            { key_t::A                 , false },  { key_t::F22               , false },  { key_t::Rctrl             , false },
            { key_t::B                 , false },  { key_t::F23               , false },  { key_t::Rshift            , false },
            { key_t::C                 , false },  { key_t::F24               , false },  { key_t::Ralt              , false },
            { key_t::D                 , false },  { key_t::Execute           , false },  { key_t::Rgui              , false },
            { key_t::E                 , false },  { key_t::Help              , false },  { key_t::Mode              , false },
            { key_t::F                 , false },  { key_t::Menu              , false },  { key_t::Audionext         , false },
            { key_t::G                 , false },  { key_t::Select            , false },  { key_t::Audioprev         , false },
            { key_t::H                 , false },  { key_t::Stop              , false },  { key_t::Audiostop         , false },
            { key_t::I                 , false },  { key_t::Again             , false },  { key_t::Audioplay         , false },
            { key_t::J                 , false },  { key_t::Undo              , false },  { key_t::Audiomute         , false },
            { key_t::K                 , false },  { key_t::Cut               , false },  { key_t::Mediaselect       , false },
            { key_t::L                 , false },  { key_t::Copy              , false },  { key_t::Www               , false },
            { key_t::M                 , false },  { key_t::Paste             , false },  { key_t::Mail              , false },
            { key_t::N                 , false },  { key_t::Find              , false },  { key_t::Calculator        , false },
            { key_t::O                 , false },  { key_t::Mute              , false },  { key_t::Computer          , false },
            { key_t::P                 , false },  { key_t::Volumeup          , false },  { key_t::Ac_Search         , false },
            { key_t::Q                 , false },  { key_t::Volumedown        , false },  { key_t::Ac_Home           , false },
            { key_t::R                 , false },  { key_t::Kp_Comma          , false },  { key_t::Ac_Back           , false },
            { key_t::S                 , false },  { key_t::Kp_Equalsas400    , false },  { key_t::Ac_Forward        , false },
            { key_t::T                 , false },  { key_t::Alterase          , false },  { key_t::Ac_Stop           , false },
            { key_t::U                 , false },  { key_t::Sysreq            , false },  { key_t::Ac_Refresh        , false },
            { key_t::V                 , false },  { key_t::Cancel            , false },  { key_t::Ac_Bookmarks      , false },
            { key_t::W                 , false },  { key_t::Clear             , false },  { key_t::Brightnessdown    , false },
            { key_t::X                 , false },  { key_t::Prior             , false },  { key_t::Brightnessup      , false },
            { key_t::Y                 , false },  { key_t::Return2           , false },  { key_t::Displayswitch     , false },
            { key_t::Z                 , false },  { key_t::Separator         , false },  { key_t::Kbdillumtoggle    , false },
            { key_t::Capslock          , false },  { key_t::Kout              , false },  { key_t::Kbdillumdown      , false },
            { key_t::F1                , false },  { key_t::Oper              , false },  { key_t::Kbdillumup        , false },
            { key_t::F2                , false },  { key_t::Clearagain        , false },  { key_t::Eject             , false },
            { key_t::F3                , false },  { key_t::Crsel             , false },  { key_t::Sleep             , false },
            { key_t::F4                , false },  { key_t::Exsel             , false },  { key_t::App1              , false },
            { key_t::F5                , false },  { key_t::Kp_00             , false },  { key_t::App2              , false },
            { key_t::F6                , false },  { key_t::Kp_000            , false },  { key_t::Audiorewind       , false },
            { key_t::F7                , false },  { key_t::Thousandsseparator, false },  { key_t::Audiofastforward  , false },
            { key_t::F8                , false },  { key_t::Decimalseparator  , false },  { key_t::F9                , false },  
            { key_t::Currencyunit      , false },  { key_t::F10               , false },  { key_t::Currencysubunit   , false },  
        };
        
        m_actButtonMap ={
            { button_t::Left    , false }, 
            { button_t::Right   , false },
            { button_t::Middle  , false }
        };

        for (auto& [keyCode, _] : m_actKeyMap) {
            m_keyStates[keyCode] = {false, false, false};
        }

        for (auto& [buttonCode, _] : m_actButtonMap) {
            m_buttonStates[buttonCode] = {false, false, false};
        }

    }

    input_manager::~input_manager() {}


    void input_manager::update() {
        for (auto& [keyCode, status] : m_actKeyMap) {
            m_keyStates[keyCode].bPressed = false;
            m_keyStates[keyCode].bReleased = false;

            if (status != m_prevKeyMap[keyCode]) {
                if (status) {
                    m_keyStates[keyCode].bPressed = !m_keyStates[keyCode].bHeld;
                    m_keyStates[keyCode].bHeld = true;
                }
                else {
                    m_keyStates[keyCode].bReleased = true;
                    m_keyStates[keyCode].bHeld = false;
                }
            }
            m_prevKeyMap[keyCode] = status;
        }

        for (auto& [buttonCode, status] : m_actButtonMap) {
            m_buttonStates[buttonCode].bPressed = false;
            m_buttonStates[buttonCode].bReleased = false;

            if (status != m_prevButtonMap[buttonCode]) {
                if (status) {
                    m_buttonStates[buttonCode].bPressed = !m_buttonStates[buttonCode].bHeld;
                    m_buttonStates[buttonCode].bHeld = true;
                }
                else {
                    m_buttonStates[buttonCode].bReleased = true;
                    m_buttonStates[buttonCode].bHeld = false;
                }
            }
            m_prevButtonMap[buttonCode] = status;
        }

        if (! m_scrollUpdated) {
            m_verticalScroll = 0;
            m_horizontalScroll = 0;
        }
        m_scrollUpdated = false;
    }


    bool input_manager::keyHeld(key_t keyId) const {
        if (auto it = m_keyStates.find(keyId); it != m_keyStates.end()) {
            return it->second.bHeld;
        }
        return false;
    }

    bool input_manager::keyPressed(key_t keyId) const {
        if (auto it = m_keyStates.find(keyId); it != m_keyStates.end()) {
            return it->second.bPressed;
        }
        return false;
    }

    bool input_manager::keyReleased(key_t keyId) const {
        if (auto it = m_keyStates.find(keyId); it != m_keyStates.end()) {
            return it->second.bReleased;
        }
        return false;
    }

    bool input_manager::buttonHeld(button_t buttonId) const {
        if (auto it = m_buttonStates.find(buttonId); it != m_buttonStates.end()) {
            return it->second.bHeld;
        }
        return false;
    }

    bool input_manager::buttonPressed(button_t buttonId) const {
        if (auto it = m_buttonStates.find(buttonId); it != m_buttonStates.end()) {
            return it->second.bPressed;
        }
        return false;
    }

    bool input_manager::buttonReleased(button_t buttonId) const {
        if (auto it = m_buttonStates.find(buttonId); it != m_buttonStates.end()) {
            return it->second.bReleased;
        }
        return false;
    }


    int32_t input_manager::verticalScroll() const {
        return m_verticalScroll;
    }

    int32_t input_manager::horizontalScroll() const {
        return m_horizontalScroll;
    }

    math::vec2di input_manager::mousePos() const {
        return m_mouseCords;
    }

    int32_t input_manager::mouseX() const {
        return m_mouseCords.x;
    }

    int32_t input_manager::mouseY() const {
        return m_mouseCords.y;
    }

    void input_manager::keyPress(const int32_t& keyId) {
        m_actKeyMap[static_cast<key_t>(keyId)] = true;
    }

    void input_manager::keyRelease(const int32_t& keyId) {
        m_actKeyMap[static_cast<key_t>(keyId)] = false;
    }

    void input_manager::buttonPress(const Uint8& buttonId) {
        m_actButtonMap[static_cast<button_t>(buttonId)] = true;
    }

    void input_manager::buttonRelease(const Uint8& buttonId) {
        m_actButtonMap[static_cast<button_t>(buttonId)] = false;
    }

    void input_manager::verticalScroll(const int32_t& val) {
        m_verticalScroll = val;
        m_scrollUpdated = true;
    }

    void input_manager::horizontalScroll(const int32_t& val) {
        m_horizontalScroll = val;
        m_scrollUpdated = true;
    }

    void input_manager::mouseCoords(const math::vec2di &coords) {
        m_mouseCords = coords;
    }

}