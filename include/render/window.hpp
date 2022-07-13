#pragma once

#include <string>
#include <string_view>

#include <SDL2/SDL.h>

#include <math/vec2d.hpp>

namespace arti::render {

    using window_centered = std::integral_constant<uint32_t, SDL_WINDOWPOS_CENTERED>;

    namespace window_type {
        enum : uint32_t {
            Fullscreen        = SDL_WINDOW_FULLSCREEN,
            OpenGL            = SDL_WINDOW_OPENGL,
            Borderless        = SDL_WINDOW_BORDERLESS,
            Resizable         = SDL_WINDOW_RESIZABLE,
            FullscreenDesktop = SDL_WINDOW_FULLSCREEN_DESKTOP,
            AllowHighDPI      = SDL_WINDOW_ALLOW_HIGHDPI,
            AlwaysOnTop       = SDL_WINDOW_ALWAYS_ON_TOP
        };
    }

    class app;

    class window {
    
    friend class app;

    public:
        window();
        ~window();

        bool init(std::string_view name, math::vec2di size, math::vec2di pos, uint32_t flags);
        bool init(std::string_view name, math::vec2di size, uint32_t flags);

        int32_t getWidth() const;
        int32_t getHeight() const;
        math::vec2di getSize() const;
        const std::string& getName() const;

        void setName(std::string_view name);
        void setSize(math::vec2di size);

        SDL_Window* get();
        SDL_GLContext getContext();

    private:
        SDL_Window* m_window;
        SDL_GLContext m_glContext;
        math::vec2di m_currentSize;
        math::vec2di m_requestedSize;
        std::string m_name;
    
    };
}