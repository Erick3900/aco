#include <render/window.hpp>

namespace arti::render {

    window::window()
      : m_window(nullptr),
        m_glContext(nullptr),
        m_currentSize({0, 0}),
        m_requestedSize({480, 480}),
        m_name("Artichaut window") {

    }

    window::~window() {
        if (m_glContext)
            SDL_GL_DeleteContext(m_glContext);
        
        if (m_window)
            SDL_DestroyWindow(m_window);

        m_glContext = nullptr;
        m_window = nullptr;
    }
    
    bool window::init(std::string_view name, math::vec2di size, math::vec2di pos, uint32_t flags) {
        m_name = name;
        m_requestedSize = size;
        
        m_window = SDL_CreateWindow(
            m_name.c_str(),
            pos.x, pos.y,
            size.x, size.y,
            flags
        );

        if (! m_window) {
            // TODO log errors and info
            return false;
        }

        m_glContext = SDL_GL_CreateContext(m_window);

        if (! m_glContext) {
            // TODO log errors and info
            return false;
        }

        SDL_GL_MakeCurrent(m_window, m_glContext);

        SDL_GL_SetSwapInterval(0); // TODO make vsync available

        SDL_GetWindowSize(m_window, &m_currentSize.x, &m_currentSize.y);

        return true;
    }

    bool window::init(std::string_view name, math::vec2di size, uint32_t flags) {
        return init(name, size, {window_centered::value, window_centered::value}, flags);
    }

    int32_t window::getWidth() const {
        return m_currentSize.x;
    }

    int32_t window::getHeight() const {
        return m_currentSize.y;
    }

    math::vec2di window::getSize() const {
        return m_currentSize;
    }

    const std::string &window::getName() const {
        return m_name;
    }

    void window::setName(std::string_view name) {
        m_name = name;
        SDL_SetWindowTitle(m_window, m_name.c_str());
    }

    void window::setSize(math::vec2di size) {
        m_requestedSize = size;
        SDL_SetWindowSize(m_window, m_requestedSize.x, m_requestedSize.y);
        SDL_GetWindowSize(m_window, &m_currentSize.x, &m_currentSize.y);
    }

    SDL_Window *window::get() {
        return m_window;
    }

    SDL_GLContext window::getContext() {
        return m_glContext;
    }

}