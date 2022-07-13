#pragma once

#include <math/base.hpp>

namespace arti::math::constants {

    static inline const constexpr math::real EPS = 1e-9;
    static inline const constexpr math::real PI = 3.1415926535897932384626433;
    static inline const constexpr math::real E = 2.7182818284590452353602874;
    static inline const constexpr math::real toRads = PI / 180.0;
    static inline const constexpr math::real toDegs = 180.0 / PI;

    static inline const constexpr math::integer MOD = 1e9 + 7;

}