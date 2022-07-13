cmake_minimum_required(VERSION 3.20)

include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 8.1.1
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/deps/fmt"
)

FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.10.5/json.hpp
    DOWNLOAD_NO_EXTRACT TRUE
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/deps/nlohmann"
)

FetchContent_MakeAvailable(json)

project(AntColonyVisualization)

set(CMAKE_CXX_STANDARD 17)

add_executable(
    AntColonyVisualization
        main.cpp
        src/render/app.cpp
        src/render/basic_renderer.cpp
        src/render/init.cpp
        src/render/input_manager.cpp
        src/render/pixel.cpp
        src/render/window.cpp
        src/aco/ant.cpp
        src/ant_visualization.cpp
        deps/imgui/src/imgui.cpp
        deps/imgui/src/imgui_demo.cpp
        deps/imgui/src/imgui_draw.cpp
        deps/imgui/src/imgui_tables.cpp
        deps/imgui/src/imgui_stdlib.cpp
        deps/imgui/src/imgui_widgets.cpp
        deps/imgui/src/imgui_impl_sdl.cpp
        deps/imgui/src/imgui_impl_sdlrenderer.cpp
)

target_include_directories(
    AntColonyVisualization PUBLIC
        include/
        deps/fmt/include
        deps/nlohmann
        deps/imgui
)


if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(
        AntColonyVisualization PUBLIC
            SDL2
            SDL2_gfx
            SDL2_image
            GL
            fmt
    )
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    target_link_libraries(
        AntColonyVisualization PUBLIC
            mingw32
            SDL2main
            SDL2
            SDL2_gfx
            SDL2_image
            opengl32
            fmt
    )
endif()

set_target_properties(
    AntColonyVisualization PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
