#pragma once

#include <list>

#include <math/uwd_graph.hpp>

namespace arti::aco {
    using graph_t = math::uwd_graph<double>;

    struct ant {
        ant(graph_t &g);
        
        graph_t::node_id currNode();
        void reset();
        void visitNode(graph_t::node_id nodeId);
        double distanceTraveled();

        uint64_t id;
        bool stuck;
        bool precalcDistance;
        double traveledDistance;
        std::list<graph_t::node_id> path;
        std::unordered_map<graph_t::node_id, bool> visited;
        
        graph_t &graph;
    };
}
