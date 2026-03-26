#pragma once

#include "../Types.hpp"
#include "../concepts/concepts.hpp"

namespace ailib {

    // Default implementation of Exploration Solver tracker
    template <typename S, typename A>
    struct DefaultExplorationTracker {
        // --- Result Data ---
        bool goal_found = false;
        std::vector<A> plan;
        double score = 0.0;
        
        // --- Metrics ---
        std::size_t expanded_nodes = 0;
        std::size_t generated_nodes = 0;
        std::size_t max_frontier_size = 0;
        int max_depth = 0;

        // --- Hooks (matching ExplorationTracker concept) ---
        void on_start(const S&) { 
            // Initialize if needed
        }

        template <typename Node>
        void on_expand(const Node* node) { 
            expanded_nodes++; 
            if (node->depth > max_depth) max_depth = node->depth;
        }

        template <typename Node>
        void on_generate(const Node*) { 
            generated_nodes++; 
        }

        void on_frontier_update(std::size_t size) {
            if (size > max_frontier_size) max_frontier_size = size;
        }

        template <typename Node>
        void on_finish(const Node* goal_node) {
            if (goal_node != nullptr) {
                goal_found = true;
                score = goal_node->path_cost;
                
                auto current = goal_node;
                while (current->parent != nullptr) {
                    plan.push_back(current->action);
                    current = current->parent;
                }
                std::reverse(plan.begin(), plan.end());
            }
        }
    };

}
