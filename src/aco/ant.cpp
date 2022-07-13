#include <aco/ant.hpp>

#include <logger.hpp>

namespace arti::aco {

    ant::ant(graph_t &g) : graph(g) {
        reset();
        traveledDistance = 0.0;
        precalcDistance = true;
        stuck = false;
    }

    graph_t::node_id ant::currNode() {
        return path.back();
    }

    void ant::reset() {
        path.clear();
        stuck = false;
        visited.clear();
        traveledDistance = 0.0;
        precalcDistance = true;
        for (graph_t::node_id i = 0; i < graph.size(); ++i) {
            visited[i] = false;
        }
    }

    void ant::visitNode(graph_t::node_id nodeId) {
        if (! visited[nodeId]) {
            visited[nodeId] = true;
            precalcDistance = true;
            path.push_back(nodeId);
        }
        else {
            throw std::runtime_error("HOW THIS HAPPENED?");
        }
    }

    double ant::distanceTraveled() {
        if (stuck) return graph_t::inf;

        if (precalcDistance) {
            traveledDistance = 0.0;
            auto lIt = path.back();
            for (auto& it : path) {
                traveledDistance += graph.getWeigth(lIt, it);
                lIt = it;
            }
            precalcDistance = false;
        }
        return traveledDistance;
    }
}