#pragma once

#include <cstdint>

// TODO credits to David Barr, oneLoneCoder, code from olc::PixelGameEngine

namespace arti::render {

    struct pixel
    {
        union
        {
            uint32_t n = 0xFF;
            struct {
                uint8_t r;
                uint8_t g;
                uint8_t b;
                uint8_t a;
            };
        };

        enum Mode {
            NORMAL,
            MASK,
            ALPHA,
            CUSTOM
        };

        pixel();
        pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 0xFF);
        pixel(uint32_t p);

        pixel&  operator=   (const pixel &v ) = default;
        bool    operator==  (const pixel &p ) const;
        bool    operator!=  (const pixel &p ) const;
        pixel   operator*   (const float i  ) const;
        pixel   operator/   (const float i  ) const;
        pixel&  operator*=  (const float i  );
        pixel&  operator/=  (const float i  );
        pixel   operator+   (const pixel &p ) const;
        pixel   operator-   (const pixel &p ) const;
        pixel&  operator+=  (const pixel &p );
        pixel&  operator-=  (const pixel &p );
        pixel   inv() const;
    };

    pixel pixelF(float red, float green, float blue, float alpha = 1.0f);
    pixel pixelLerp(const pixel &p1, const pixel &p2, float t);

}