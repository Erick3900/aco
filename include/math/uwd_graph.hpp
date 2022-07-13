#pragma once

#include <map>
#include <unordered_map>
#include <vector>
#include <limits>
#include <cstdint>

namespace arti::math {

    template<typename weight_t = double>
    class undirected_weighted_dynamic_graph {

    public:
        using node_id = int32_t;
        using neighbors_list_t = std::map<node_id, weight_t>;
        static constexpr const weight_t inf = std::numeric_limits<weight_t>::max();

    public:
        undirected_weighted_dynamic_graph() {
            m_edgesCount = 0;
        }

        node_id addNode() {
            m_nodes.insert({static_cast<node_id>(m_nodes.size()), {}});
            return m_nodes.size() - 1;
        }

        void connect(node_id node_A, node_id node_B, weight_t weight) {
            if (m_nodes[node_A].find(node_B) == m_nodes[node_A].end())
                m_edgesCount++;
            m_nodes[node_A][node_B] = weight;
            m_nodes[node_B][node_A] = weight;
        }

        void disconnect(node_id node_A, node_id node_B) {
            m_nodes[node_A].erase(node_B);
            m_nodes[node_B].erase(node_A);
            m_edgesCount--;
        }

        bool areConnected(node_id node_A, node_id node_B) const {
            if (auto it = m_nodes.at(node_A).find(node_B); it != m_nodes.at(node_A).end()) {
                return true;
            }
            return false;
        }

        weight_t getWeigth(node_id node_A, node_id node_B) const {
            if (auto it = m_nodes.at(node_A).find(node_B); it != m_nodes.at(node_A).end()) {
                return it->second;
            }
            return inf;
        }

        const neighbors_list_t& getNeighbors(node_id node) const {
            return m_nodes.at(node);
        }

        int32_t size() const {
            return m_nodes.size();
        }

        int32_t edgesCount() const {
            return m_edgesCount;
        }

        void reset() {
            m_nodes.clear();
            m_edgesCount = 0;
        }

        auto begin() {
            return m_nodes.begin();
        }

        auto end() {
            return m_nodes.end();
        }

        auto begin() const {
            return m_nodes.begin();
        }

        auto end() const {
            return m_nodes.end();
        }

    private:
        int32_t m_edgesCount;
        std::map<node_id, neighbors_list_t> m_nodes;
    };

    template<typename weight_t = double>
    using uwd_graph = undirected_weighted_dynamic_graph<weight_t>;

}