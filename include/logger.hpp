#pragma once

#include <string>
#include <iostream>

#include <fmt/format.h>
#include <fmt/color.h>
#include <fmt/ostream.h>

namespace arti {

    class logger {
    
    public:
        logger(const logger&) = delete;
        logger(logger&&) = delete;

        logger& operator=(const logger&) = delete;
        logger& operator=(logger&&) = delete;

        template <typename... Args>
        static void info(const std::string &logMsg, Args... args) {
            fmt::print(
                "{} > {}\n",
                fmt::format(fmt::fg(fmt::terminal_color::bright_blue), "I"),
                fmt::format(logMsg, std::forward<Args>(args)...)
            );
        }

        template <typename... Args>
        static void warning(const std::string &logMsg, Args... args) {
            fmt::print(
                "{} > {}\n",
                fmt::format(fmt::fg(fmt::terminal_color::yellow), "W"),
                fmt::format(logMsg, std::forward<Args>(args)...)
            );
        }

        template <typename... Args>
        static void critical(const std::string &logMsg, Args... args) {
            fmt::print(
                "{} > {}\n",
                fmt::format(fmt::bg(fmt::terminal_color::red) | fmt::fg(fmt::terminal_color::white), "C"),
                fmt::format(logMsg, std::forward<Args>(args)...)
            );
        }

        template <typename... Args>
        static void debug(const std::string &logMsg, Args... args) {
            fmt::print(
                "{} > {}\n",
                fmt::format(fmt::fg(fmt::color::gray), "D"),
                fmt::format(logMsg, std::forward<Args>(args)...)
            );
        }

        template <typename... Args>
        static void error(const std::string &logMsg, Args... args) {
            fmt::print(
                "{} > {}\n",
                fmt::format(fmt::fg(fmt::terminal_color::red), "E"),
                fmt::format(logMsg, std::forward<Args>(args)...)
            );
        }

        template <typename... Args>
        static void print(const std::string &logMsg, Args... args) {
            using namespace std::string_literals;
            fmt::print("  > "s + logMsg + "\n", std::forward<Args>(args)...);
        }

        template <typename ...Args>
        static void print(const fmt::text_style &ts, const std::string& logMsg, Args... args) {
            using namespace std::string_literals;
            fmt::print("  > "s);
            fmt::print(ts, logMsg + "\n", std::forward<Args>(args) ...);
        }

        static void endl() {
            fmt::print("\n");
        }
    };

}