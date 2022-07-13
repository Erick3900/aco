#include <render/pixel.hpp>

#include <algorithm>

namespace arti::render {

    pixel::pixel() {
        r = 0;
        g = 0;
        b = 0;
        a = 0xFF;
    }

    pixel::pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
        n = red | (green << 8) | (blue << 16) | (alpha << 24);
    } // Thanks jarekpelczar

    pixel::pixel(uint32_t p) {
        n = p;
    }

    bool pixel::operator==(const pixel &p) const {
        return n == p.n;
    }

    bool pixel::operator!=(const pixel &p) const {
        return n != p.n;
    }

    pixel pixel::operator*(const float i) const {
        float fR = std::min(255.0f, std::max(0.0f, float(r) * i));
        float fG = std::min(255.0f, std::max(0.0f, float(g) * i));
        float fB = std::min(255.0f, std::max(0.0f, float(b) * i));
        return pixel(uint8_t(fR), uint8_t(fG), uint8_t(fB), a);
    }

    pixel pixel::operator/(const float i) const {
        float fR = std::min(255.0f, std::max(0.0f, float(r) / i));
        float fG = std::min(255.0f, std::max(0.0f, float(g) / i));
        float fB = std::min(255.0f, std::max(0.0f, float(b) / i));
        return pixel(uint8_t(fR), uint8_t(fG), uint8_t(fB), a);
    }

    pixel &pixel::operator*=(const float i) {
        this->r = uint8_t(std::min(255.0f, std::max(0.0f, float(r) * i)));
        this->g = uint8_t(std::min(255.0f, std::max(0.0f, float(g) * i)));
        this->b = uint8_t(std::min(255.0f, std::max(0.0f, float(b) * i)));
        return *this;
    }

    pixel &pixel::operator/=(const float i) {
        this->r = uint8_t(std::min(255.0f, std::max(0.0f, float(r) / i)));
        this->g = uint8_t(std::min(255.0f, std::max(0.0f, float(g) / i)));
        this->b = uint8_t(std::min(255.0f, std::max(0.0f, float(b) / i)));
        return *this;
    }

    pixel pixel::operator+(const pixel &p) const {
        uint8_t nR = uint8_t(std::min(255, std::max(0, int(r) + int(p.r))));
        uint8_t nG = uint8_t(std::min(255, std::max(0, int(g) + int(p.g))));
        uint8_t nB = uint8_t(std::min(255, std::max(0, int(b) + int(p.b))));
        return pixel(nR, nG, nB, a);
    }

    pixel pixel::operator-(const pixel &p) const {
        uint8_t nR = uint8_t(std::min(255, std::max(0, int(r) - int(p.r))));
        uint8_t nG = uint8_t(std::min(255, std::max(0, int(g) - int(p.g))));
        uint8_t nB = uint8_t(std::min(255, std::max(0, int(b) - int(p.b))));
        return pixel(nR, nG, nB, a);
    }

    pixel &pixel::operator+=(const pixel &p) {
        this->r = uint8_t(std::min(255, std::max(0, int(r) + int(p.r))));
        this->g = uint8_t(std::min(255, std::max(0, int(g) + int(p.g))));
        this->b = uint8_t(std::min(255, std::max(0, int(b) + int(p.b))));
        return *this;
    }

    pixel &pixel::operator-=(const pixel &p) {
        this->r = uint8_t(std::min(255, std::max(0, int(r) - int(p.r))));
        this->g = uint8_t(std::min(255, std::max(0, int(g) - int(p.g))));
        this->b = uint8_t(std::min(255, std::max(0, int(b) - int(p.b))));
        return *this;
    }

    pixel pixel::inv() const {
        uint8_t nR = uint8_t(std::min(255, std::max(0, 255 - int(r))));
        uint8_t nG = uint8_t(std::min(255, std::max(0, 255 - int(g))));
        uint8_t nB = uint8_t(std::min(255, std::max(0, 255 - int(b))));
        return pixel(nR, nG, nB, a);
    }

    pixel pixelF(float red, float green, float blue, float alpha) {
        return pixel(uint8_t(red * 255.0f), uint8_t(green * 255.0f), uint8_t(blue * 255.0f), uint8_t(alpha * 255.0f));
    }

    pixel pixelLerp(const pixel &p1, const pixel &p2, float t) {
        return (p2 * t) + p1 * (1.0f - t);
    }
}