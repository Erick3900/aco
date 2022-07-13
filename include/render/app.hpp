#pragma once 

#include <memory>
#include <string>

#include <SDL2/SDL.h>

#include <render/window.hpp>
#include <render/input_manager.hpp>
#include <render/basic_renderer.hpp>

namespace arti::render {

    class app {

    public:
        app();
        virtual ~app();

        bool init(std::string_view name,
                  math::vec2di windowSize,
                  math::vec2di windowPos = {window_centered::value, window_centered::value},
                  uint32_t windowFlags = window_type::OpenGL);

        int run();

        virtual bool onInit();
        virtual bool onUpdate(double) = 0;
        virtual void onRender();
        virtual bool onExit();

        virtual void onPollEvent(const SDL_Event& event);

        window wind;
        input_manager input;
        basic_renderer renderer;

    private:
        void onSDLEvent(const SDL_Event& event);

        bool pUpdate();
        bool pRender();

        void pExit();

        bool m_running;
        bool m_initialized;
        bool m_exitRequested;
        double m_deltaTime;
    };

}