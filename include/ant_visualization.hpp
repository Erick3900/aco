#pragma once

#include <list>
#include <iostream>
#include <iomanip>
#include <optional>
#include <unordered_set>
#include <algorithm>

#include <render/app.hpp>
#include <render/colors.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/backends/imgui_impl_sdl.h>
#include <imgui/backends/imgui_impl_sdlrenderer.h>

#include <logger.hpp>
#include <utils/rand.hpp>
#include <math/uwd_graph.hpp>

#include <aco/ant.hpp>

using arti::render::key_t;
using arti::render::button_t;
using arti::render::basic_renderer;
using arti::aco::graph_t;
namespace color = arti::render::colors;


namespace arti {

    class AntVisualization : public render::app {

    public:

        bool onInit() override;
        void onPollEvent(const SDL_Event& event) override;
        bool onUpdate(double delta) override;
        bool onExit() override;
        void updateStaticLayer();
        void resetAlgo();

    private:
        basic_renderer::layer_id guiLayer;
        basic_renderer::layer_id movingLayer;

        float zoom;
        math::vec2df panning;

        float accTime;

        graph_t g;
        graph_t ph;

        std::list<aco::ant> ants;

        std::list<graph_t::node_id> bestPathSoFar;
        double bestPathSoFarLength;

        double actBestPath;

        std::map<graph_t::node_id, math::vec2df> nodesPos;
        double node_size = 8.0;

        int nAlgoStep;

        int nAnts = 10;

        double alpha = 1.5;
        double beta = 1.35;
        double rho = 0.02;

        double maxPheromone = 1.0;

        bool addNode;
        bool algoStep;
        bool autoRun;
        bool showAllEdges;
        bool skipGraphics;

        bool editMode;
        std::optional<graph_t::node_id> chosenNode;

        bool modalOpen;
        int numberOfNodes;

        std::string filename;
        int radioGroup;

        std::map<graph_t::node_id, double> weightsHelper;
    };

}