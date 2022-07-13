#include <render/init.hpp>

#include <ant_visualization.hpp>

using namespace arti;

int main(int argc, char** argv) {
    render::initSDL(render::init_flags::Everything);

    AntVisualization app;
    
    if (app.init("Ant visualization", {860, 640})) {
        app.run();
    }

    render::closeSDL();
    return 0;
}