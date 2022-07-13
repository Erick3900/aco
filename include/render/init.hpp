#pragma once

#include <cstdint>
#include <initializer_list>

#include <SDL2/SDL.h>

namespace arti::render {

    namespace init_flags {
        enum : uint32_t {
            Timer = SDL_INIT_TIMER,
            Audio = SDL_INIT_AUDIO,
            Video = SDL_INIT_VIDEO,
            Everything = SDL_INIT_EVERYTHING
        };

    }

    void initSDL(const uint32_t &initFlags, std::initializer_list<std::pair<SDL_GLattr, int>> attributes = {});

    void setSDLAttributes(std::initializer_list<std::pair<SDL_GLattr, int>> attributes);

    void closeSDL();

}