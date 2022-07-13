#pragma once

#include <random>

namespace arti::random {

    static double f_zero_to_one() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<> dist(0, 1.0);

        return dist(gen);
    }

    static double f_neg_to_pos_one() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<> dist(-1.0, 1.0);

        return dist(gen);
    }

    static int32_t i_zero_intMax() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dist(0, INT32_MAX);

        return dist(gen);
    }

}