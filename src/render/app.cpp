#include <render/app.hpp>

namespace arti::render {

    app::app()
      : m_running(false),
        m_initialized(false),
        m_exitRequested(false),
        m_deltaTime(0.0),
        wind(),
        input(),
        renderer(&wind) {
        
    }

    app::~app() {
        // TODO log errors and info;
        pExit();
    }

    bool app::onInit() { return true; }
    bool app::onExit() { return true; }
    void app::onRender() { }

    void app::onPollEvent(const SDL_Event& event) { }

    bool app::init(std::string_view name, math::vec2di windowSize, math::vec2di windowPos, uint32_t windowFlags) {
        if (! wind.init(name, windowSize, windowPos, windowFlags)) {
            // TODO log errors and info
            return false;
        }

        if (! renderer.init()) {
            // TODO log errors and info
            return false;
        }

        if (! onInit()) {
            // TODO log errors and info
            return false;
        }
        m_initialized = true;
        return true;
    }

    int app::run() {
        if (! m_initialized) {
            // TODO log errors and info
            return EXIT_FAILURE;
        }

        m_running = true;

        auto actTime = SDL_GetTicks64();
        auto lastTime = actTime;

        while (m_running) {
            actTime = SDL_GetTicks64();
            if (!m_exitRequested && pUpdate()) {
                pRender();
                m_deltaTime = static_cast<double>(actTime - lastTime) / 1000.0;
                lastTime = actTime;
            }
            else {
                pExit();
            }
        }

        return EXIT_SUCCESS;
    }

    void app::onSDLEvent(const SDL_Event &event) {
        // TODO log errors and info
        switch (event.type) {
            case SDL_QUIT:
                m_exitRequested = true;
                break;
            case SDL_MOUSEMOTION:
                input.mouseCoords({
                    event.motion.x,
                    event.motion.y
                });
                break;
            case SDL_KEYDOWN:
                input.keyPress(event.key.keysym.sym);
                break;
            case SDL_KEYUP:
                input.keyRelease(event.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:
                input.buttonPress(event.button.button);
                break;
            case SDL_MOUSEBUTTONUP:
                input.buttonRelease(event.button.button);
                break;
            case SDL_MOUSEWHEEL:
                input.verticalScroll(event.wheel.y);
                input.horizontalScroll(event.wheel.x);
                break;
            default:
                // TODO handle other events
                break;
        }
    }

    bool app::pUpdate() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            onPollEvent(event);
            onSDLEvent(event);
        }
        input.update();

        if (m_exitRequested)
            return true;

        if (! onUpdate(m_deltaTime)) {
            // TODO log errors and info
            return false;
        }

        return true;
    }

    // TODO check missing?
    bool app::pRender() {
        onRender();
        renderer.render();
        return true;
    }
    
    void app::pExit() {
        if (! m_running) return;

        m_running = false;
        if (! onExit()) {
            // TODO log errors and info
        }
    }

}