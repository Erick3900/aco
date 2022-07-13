#include <render/basic_renderer.hpp>

#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <logger.hpp>

namespace arti::render {

    basic_renderer::basic_renderer(window *windowInstance)
     : m_currLayer(0), 
       m_currTexture(0),
       m_defaultLayer(0),
       m_targetedLayer(0),
       m_windowInstance(windowInstance) {
        if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0) {
            throw std::runtime_error("Couldn't init SDL_Image");
        }
    }

    basic_renderer::~basic_renderer() {
        for (auto& [layerId, layer] : m_layers) {
            SDL_DestroyTexture(layer.texture);
        }
        if (m_renderer) {
            SDL_DestroyRenderer(m_renderer);
        }
    }

    void basic_renderer::clear(const pixel &color) {
        SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(m_renderer);
    }


    basic_renderer::layer_id basic_renderer::createLayer() {
        auto texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB4444, SDL_TEXTUREACCESS_TARGET, m_windowInstance->getWidth(), m_windowInstance->getHeight());
        if (texture == NULL) {
            return UINT16_MAX;
        }

        if (SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND) != 0) {
            SDL_DestroyTexture(texture);
            return UINT16_MAX;
        }

        pixel p;

        SDL_GetRenderDrawColor(m_renderer, &p.r, &p.g, &p.b, &p.a);
        SDL_SetRenderTarget(m_renderer, texture);
        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
        SDL_RenderClear(m_renderer);
        SDL_SetRenderTarget(m_renderer, m_layers[m_targetedLayer].texture);
        SDL_SetRenderDrawColor(m_renderer, p.r, p.g, p.b, p.a);

        ++m_currLayer;
        m_layers[m_currLayer].id = m_currLayer;
        m_layers[m_currLayer].enabled = true;
        m_layers[m_currLayer].texture = texture;
        m_layers[m_currLayer].scale = {1.0f, 1.0f};
        m_layers[m_currLayer].position = {0.0f, 0.0f};
        m_layers[m_currLayer].size = m_windowInstance->getSize();
        return m_currLayer;
    }

    basic_renderer::layer_id basic_renderer::createLayer(math::vec2di size) {
        auto texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB4444, SDL_TEXTUREACCESS_TARGET, size.x, size.y);
        if (texture == NULL) {
            return UINT16_MAX;
        }

        if (SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND) != 0) {
            SDL_DestroyTexture(texture);
            return UINT16_MAX;
        }

        pixel p;

        SDL_GetRenderDrawColor(m_renderer, &p.r, &p.g, &p.b, &p.a);
        SDL_SetRenderTarget(m_renderer, texture);
        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
        SDL_RenderClear(m_renderer);
        SDL_SetRenderTarget(m_renderer, m_layers[m_targetedLayer].texture);
        SDL_SetRenderDrawColor(m_renderer, p.r, p.g, p.b, p.a);

        ++m_currLayer;
        m_layers[m_currLayer].id = m_currLayer;
        m_layers[m_currLayer].enabled = true;
        m_layers[m_currLayer].texture = texture;
        m_layers[m_currLayer].scale = {1.0f, 1.0f};
        m_layers[m_currLayer].position = {0.0f, 0.0f};
        m_layers[m_currLayer].size = size;
        return m_currLayer;
    }

    void basic_renderer::offsetLayer(math::vec2df pos) {
        m_layers[m_targetedLayer].position += pos;
    }

    void basic_renderer::scaleLayer(math::vec2df scale, math::vec2df center)
    {
        math::vec2df before {
            (center.x - m_layers[m_targetedLayer].position.x) / m_layers[m_targetedLayer].scale.x,
            (center.y - m_layers[m_targetedLayer].position.y) / m_layers[m_targetedLayer].scale.y
        };

        math::vec2df after {
            m_layers[m_targetedLayer].position.x + (before.x * scale.x),
            m_layers[m_targetedLayer].position.y + (before.y * scale.y)
        };

        m_layers[m_targetedLayer].scale = scale;

        m_layers[m_targetedLayer].position -= (after - center);
    }

    math::vec2df basic_renderer::getLayerScale() const {
        return m_layers.at(m_targetedLayer).scale;
    }

    math::vec2df basic_renderer::getLayerOffset() const {
        return m_layers.at(m_targetedLayer).position;
    }

    math::vec2df basic_renderer::getLayerSize() const {
        return m_layers.at(m_targetedLayer).size;
    }

    void basic_renderer::resizeLayer(math::vec2di size) {
        auto texture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB4444, SDL_TEXTUREACCESS_TARGET, size.x, size.y);
        if (texture == NULL) {
            return;
        }

        if (SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND) != 0) {
            SDL_DestroyTexture(texture);
            return;
        }

        pixel p;

        SDL_GetRenderDrawColor(m_renderer, &p.r, &p.g, &p.b, &p.a);
        SDL_SetRenderTarget(m_renderer, texture);
        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
        SDL_RenderClear(m_renderer);
        SDL_SetRenderTarget(m_renderer, m_layers[m_targetedLayer].texture);
        SDL_SetRenderDrawColor(m_renderer, p.r, p.g, p.b, p.a);

        auto oldTexture = m_layers[m_targetedLayer].texture;

        m_layers[m_targetedLayer].texture = texture;
        m_layers[m_targetedLayer].scale = {1.0f, 1.0f};
        m_layers[m_targetedLayer].position = {0.0f, 0.0f};
        m_layers[m_targetedLayer].size = size;
        
        SDL_DestroyTexture(oldTexture);
    }

    bool basic_renderer::enableLayer(const layer_id& id, bool enabled) {
        if (auto it = m_layers.find(id); it != m_layers.end()) {
            it->second.enabled = enabled;
            return true;
        }
        return false;
    }

    bool basic_renderer::isLayerEnabled(const layer_id &id) {
        if (auto it = m_layers.find(id); it != m_layers.end()) {
            return it->second.enabled;
        }
        return false;
    }


    bool basic_renderer::setTargetedLayer(const layer_id &id) {
        if (m_layers.find(id) != m_layers.end()) {
            m_targetedLayer = id;
            SDL_SetRenderTarget(m_renderer, m_layers[m_targetedLayer].texture);
            return  true;
        }
        return false;
    }
    
    basic_renderer::layer_id basic_renderer::getTargetedLayer() const {
        return m_targetedLayer;
    }

    bool basic_renderer::targetDefaultLayer() {
        return this->setTargetedLayer(m_defaultLayer);
    }


    void basic_renderer::drawPixel(const math::vec2ds &coord, const pixel &color) {
        pixelRGBA(m_renderer, coord.x, coord.y, color.r, color.g, color.b, color.a);
    }


    void basic_renderer::drawCircle(const math::vec2ds &centerCoord, int16_t radius, const pixel &color) {
        circleRGBA(m_renderer, centerCoord.x, centerCoord.y, radius, color.r, color.g, color.b, color.a);
    }

    void basic_renderer::fillCircle(const math::vec2ds &centerCoord, int16_t radius, const pixel &color) {
        filledCircleRGBA(m_renderer, centerCoord.x, centerCoord.y, radius, color.r, color.g, color.b, color.a);
    }


    void basic_renderer::drawHLine(const math::vec2ds &startCoord, int16_t length, const pixel &color) {
        hlineRGBA(m_renderer, startCoord.x, startCoord.x + length, startCoord.y, color.r, color.g, color.b, color.a);
    }

    void basic_renderer::drawVLine(const math::vec2ds &startCoord, int16_t length, const pixel &color) {
        vlineRGBA(m_renderer, startCoord.x, startCoord.y, startCoord.y + length, color.r, color.g, color.b, color.a);
    }

    void basic_renderer::drawLine(const math::vec2ds &startCoord, const math::vec2ds &endCoord, const pixel &color) {
        lineRGBA(m_renderer, startCoord.x, startCoord.y, endCoord.x, endCoord.y, color.r, color.g, color.b, color.a);
    }

    void basic_renderer::drawThickLine(const math::vec2ds &startCoord, const math::vec2ds &endCoord, int16_t thickness, const pixel &color) {
        thickLineRGBA(m_renderer, startCoord.x, startCoord.y, endCoord.x, endCoord.y, thickness, color.r, color.g, color.b, color.a);
    }


    void basic_renderer::drawRectangle(const math::vec2ds &supLeftCorner, const math::vec2ds &botRigthCorner, const pixel &color) {
        rectangleRGBA(m_renderer, supLeftCorner.x, supLeftCorner.y, botRigthCorner.x, botRigthCorner.y, color.r, color.g, color.b, color.a);
    }

    void basic_renderer::fillRectangle(const math::vec2ds &supLeftCorner, const math::vec2ds &botRigthCorner, const pixel &color) {
        boxRGBA(m_renderer, supLeftCorner.x, supLeftCorner.y, botRigthCorner.x, botRigthCorner.y, color.r, color.g, color.b, color.a);
    }


    void basic_renderer::drawRoundedRectangle(const math::vec2ds &supLeftCorner, const math::vec2ds &botRigthCorner, int16_t radius, const pixel &color) {
        roundedRectangleRGBA(m_renderer, supLeftCorner.x, supLeftCorner.y, botRigthCorner.x, botRigthCorner.y, radius, color.r, color.g, color.b, color.a);
    }

    void basic_renderer::fillRoundedRectangle(const math::vec2ds &supLeftCorner, const math::vec2ds &botRigthCorner, int16_t radius, const pixel &color) {
        roundedBoxRGBA(m_renderer, supLeftCorner.x, supLeftCorner.y, botRigthCorner.x, botRigthCorner.y, radius, color.r, color.g, color.b, color.a);
    }


    void basic_renderer::drawEllipse(const math::vec2ds &centerCoord, const math::vec2ds &radius, const pixel &color) {
        ellipseRGBA(m_renderer, centerCoord.x, centerCoord.y, radius.x, radius.y, color.r, color.g, color.b, color.a);
    }

    void basic_renderer::fillEllipse(const math::vec2ds &centerCoord, const math::vec2ds &radius, const pixel &color) {
        filledEllipseRGBA(m_renderer, centerCoord.x, centerCoord.y, radius.x, radius.y, color.r, color.g, color.b, color.a);
    }


    void basic_renderer::drawTriangle(const math::vec2ds &coordA, const math::vec2ds &coordB, const math::vec2ds &coordC, const pixel &color) {
        trigonRGBA(m_renderer, coordA.x, coordA.y, coordB.x, coordB.y, coordC.x, coordC.y, color.r, color.g, color.b, color.a);
    }
    
    void basic_renderer::fillTriangle(const math::vec2ds &coordA, const math::vec2ds &coordB, const math::vec2ds &coordC, const pixel &color) {
        filledTrigonRGBA(m_renderer, coordA.x, coordA.y, coordB.x, coordB.y, coordC.x, coordC.y, color.r, color.g, color.b, color.a);
    }
    

    void basic_renderer::drawPolygon(const std::vector<math::vec2ds> &points, const pixel &color) {
        int16_t* x_points = new int16_t[points.size()];
        int16_t* y_points = new int16_t[points.size()];

        for (std::size_t i = 0; i < points.size(); ++i) {
            x_points[i] = points[i].x;
            y_points[i] = points[i].y;
        }

        polygonRGBA(m_renderer, x_points, y_points, points.size(), color.r, color.g, color.b, color.a);
        delete[] x_points;
        delete[] y_points;
    }
    
    void basic_renderer::fillPolygon(const std::vector<math::vec2ds> &points, const pixel &color) {
        int16_t* x_points = new int16_t[points.size()];
        int16_t* y_points = new int16_t[points.size()];

        for (std::size_t i = 0; i < points.size(); ++i) {
            x_points[i] = points[i].x;
            y_points[i] = points[i].y;
        }

        filledPolygonRGBA(m_renderer, x_points, y_points, points.size(), color.r, color.g, color.b, color.a);
        delete[] x_points;
        delete[] y_points;
    }
    

    void basic_renderer::drawBezierCurve(const std::vector<math::vec2ds> &points, int16_t steps, const pixel &color) {
        int16_t* x_points = new int16_t[points.size()];
        int16_t* y_points = new int16_t[points.size()];

        for (std::size_t i = 0; i < points.size(); ++i) {
            x_points[i] = points[i].x;
            y_points[i] = points[i].y;
        }

        bezierRGBA(m_renderer, x_points, y_points, points.size(), steps > 1 ? steps : 2, color.r, color.g, color.b, color.a);
        delete[] x_points;
        delete[] y_points;
    }
    

    void basic_renderer::drawChar(const math::vec2ds &coord, char character, const pixel &color) {
        characterRGBA(m_renderer, coord.x, coord.y, character, color.r, color.g, color.b, color.a);
    }
    
    void basic_renderer::drawString(const math::vec2ds &coord, const std::string& text, const pixel &color) {
        stringRGBA(m_renderer, coord.x, coord.y, text.c_str(), color.r, color.g, color.b, color.a);
    }

    SDL_Renderer *basic_renderer::get() const {
        return m_renderer;
    }

    bool basic_renderer::init() {
        m_renderer = SDL_CreateRenderer(m_windowInstance->get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

        if (! m_renderer) {
            return false;
        }

        if (SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND) != 0) {
            return false;
        }

        m_defaultLayer = createLayer();

        if (m_defaultLayer == UINT16_MAX) {
            return false;
        }

        m_targetedLayer = m_defaultLayer;

        this->setTargetedLayer(m_defaultLayer);

        return true;
    }
    
    bool basic_renderer::render() {
        pixel p;

        SDL_GetRenderDrawColor(m_renderer, &p.r, &p.g, &p.b, &p.a);
        SDL_SetRenderTarget(m_renderer, NULL);
        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
        SDL_RenderClear(m_renderer);
        for (auto& [layerId, layer] : m_layers) {
            if (layer.enabled) {
                SDL_FRect viewPort = {layer.position.x, layer.position.y, layer.size.x * layer.scale.x, layer.size.y * layer.scale.y};
                SDL_RenderCopyExF(m_renderer, layer.texture, NULL, &viewPort, 0, NULL, SDL_FLIP_NONE);
            }
        }
        SDL_RenderPresent(m_renderer);

        SDL_SetRenderTarget(m_renderer, m_layers[m_targetedLayer].texture);
        SDL_SetRenderDrawColor(m_renderer, p.r, p.g, p.b, p.a);
        return true;
    }

    basic_renderer::texture_id basic_renderer::loadTexture(const std::string& fileName) {
        SDL_Surface* lSurface = IMG_Load(fileName.c_str());
        texture_t lTexture = SDL_CreateTextureFromSurface(m_renderer, lSurface);

        if (lTexture == NULL) {
            return INT16_MAX;
        }
        
        ++m_currTexture;
        m_textures[m_currTexture].id = m_currTexture;
        m_textures[m_currTexture].size = {lSurface->w, lSurface->h};
        m_textures[m_currTexture].texture = lTexture;
        m_textures[m_currTexture].filename = fileName;

        logger::info("Loaded texture '{}' of size {}", fileName, m_textures[m_currTexture].size.to_string());

        SDL_FreeSurface(lSurface);

        return m_currTexture;
    }

    void basic_renderer::renderTexture(texture_id texture, const math::vec2df &pos, const float& rotation) {
        auto& text = m_textures[texture];
        SDL_FRect renderQuad = {
            pos.x - (static_cast<float>(text.size.x) / 2.0f), 
            pos.y - (static_cast<float>(text.size.y) / 2.0f), 
            static_cast<float>(text.size.x), 
            static_cast<float>(text.size.y)
        };
        SDL_RenderCopyExF(m_renderer, m_textures[texture].texture, NULL, &renderQuad, rotation, NULL, SDL_FLIP_NONE);
    }

    void basic_renderer::renderTexture(texture_id texture, const math::vec2df &pos, const math::vec2df &size, const float& rotation) {
        SDL_FRect renderQuad = {
            pos.x - (size.x / 2.0f),
            pos.y - (size.y / 2.0f), 
            size.x, 
            size.y
        };
        SDL_RenderCopyExF(m_renderer, m_textures[texture].texture, NULL, &renderQuad, rotation, NULL, SDL_FLIP_NONE);
    }

    basic_renderer::texture_t basic_renderer::getRawTexture(texture_id texture) const {
        return m_textures.at(texture).texture;
    }
}