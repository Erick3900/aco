#pragma once

#include <map>
#include <vector>
#include <unordered_map>

#include <render/pixel.hpp>
#include <render/window.hpp>

#include <math/vec2d.hpp>

namespace arti::render {

    class app;

    class basic_renderer {

    friend class app;

    public:
        using layer_id = uint16_t;
        using texture_id = uint16_t;
        using texture_t = SDL_Texture *;

        basic_renderer(window* windowInstance);
        ~basic_renderer();

        void clear(const pixel& color);

        layer_id createLayer();
        layer_id createLayer(math::vec2di size);

        void offsetLayer(math::vec2df pos);
        void scaleLayer(math::vec2df scale, math::vec2df center);

        math::vec2df getLayerScale() const;
        math::vec2df getLayerOffset() const;
        math::vec2df getLayerSize() const;

        void resizeLayer(math::vec2di size);

        bool enableLayer(const layer_id& id, bool enabled);
        bool isLayerEnabled(const layer_id& id);

        bool setTargetedLayer(const layer_id& id);
        layer_id getTargetedLayer() const;

        bool targetDefaultLayer();

        void drawPixel(const math::vec2ds& coord, const pixel& color);

        void drawCircle(const math::vec2ds& centerCoord, int16_t radius, const pixel& color);
        void fillCircle(const math::vec2ds& centerCoord, int16_t radius, const pixel& color);

        void drawHLine(const math::vec2ds& startCoord, int16_t length, const pixel& color);
        void drawVLine(const math::vec2ds& startCoord, int16_t length, const pixel& color);
        void drawLine(const math::vec2ds& startCoord, const math::vec2ds& endCoord, const pixel& color);
        void drawThickLine(const math::vec2ds& startCoord, const math::vec2ds& endCoord, int16_t thickness,const pixel& color);

        void drawRectangle(const math::vec2ds& supLeftCorner, const math::vec2ds& botRigthCorner, const pixel& color);
        void fillRectangle(const math::vec2ds& supLeftCorner, const math::vec2ds& botRigthCorner, const pixel& color);

        void drawRoundedRectangle(const math::vec2ds& supLeftCorner, const math::vec2ds& botRigthCorner, int16_t radius, const pixel& color);
        void fillRoundedRectangle(const math::vec2ds& supLeftCorner, const math::vec2ds& botRigthCorner, int16_t radius, const pixel& color);

        void drawEllipse(const math::vec2ds& centerCoord, const math::vec2ds& radius, const pixel& color);
        void fillEllipse(const math::vec2ds& centerCoord, const math::vec2ds& radius, const pixel& color);

        void drawTriangle(const math::vec2ds& coordA, const math::vec2ds& coordB, const math::vec2ds& coordC, const pixel& color);
        void fillTriangle(const math::vec2ds& coordA, const math::vec2ds& coordB, const math::vec2ds& coordC, const pixel& color);

        void drawPolygon(const std::vector<math::vec2ds>& points, const pixel& color);
        void fillPolygon(const std::vector<math::vec2ds>& points, const pixel& color);

        void drawBezierCurve(const std::vector<math::vec2ds>& points, int16_t steps, const pixel& color);
        
        void drawChar(const math::vec2ds& coord, char character, const pixel& color);
        void drawString(const math::vec2ds &coord, const std::string& text, const pixel &color);

        texture_id loadTexture(const std::string& filename);
        
        void renderTexture(texture_id texture, const math::vec2df& pos, const float& rotation = 0.0);
        void renderTexture(texture_id texture, const math::vec2df& pos, const math::vec2df& size, const float& rotation = 0.0);
        texture_t getRawTexture(texture_id texture) const;

        SDL_Renderer* get() const;

    private:
        bool init();
        bool render();

        struct Layer {
            layer_id id;
            texture_t texture;
            bool enabled;
            math::vec2df position;
            math::vec2df scale;
            math::vec2df size;
        };

        struct texture {
            texture_id id;
            texture_t texture;
            math::vec2di size;
            std::string filename; 
        };

        layer_id m_currLayer;
        layer_id m_currTexture;
        layer_id m_defaultLayer;
        layer_id m_targetedLayer;
        window* m_windowInstance;
        SDL_Renderer* m_renderer;
        std::map<layer_id, Layer> m_layers;
        std::unordered_map<texture_id, texture> m_textures;
    };

}