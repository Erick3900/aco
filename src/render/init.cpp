#include <render/init.hpp>

#include <stdexcept>

namespace arti::render {

    void initSDL(const uint32_t &initFlags, std::initializer_list<std::pair<SDL_GLattr, int>> attributes) {
        if (SDL_Init(initFlags) != 0) {
            // TODO log errors and info
            throw std::runtime_error("Couldn't initialize SDL");
        }
        if (attributes.size() != 0) {
            setSDLAttributes(attributes);
        }
    }

    void setSDLAttributes(std::initializer_list<std::pair<SDL_GLattr, int>> attributes) {
        for (auto& [attr, val] : attributes) {
            SDL_GL_SetAttribute(attr, val);
        }
    }

    void closeSDL() {
        SDL_Quit();
    }

}