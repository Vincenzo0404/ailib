#pragma once

#include "Types.hpp"
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <deque>



namespace ailib {

    template <Problem P, Frontier<SearchNode<typename P::State, typename P::Action>> F>
    // Agent for state space exploration problems
    class StateSearchAgent {
    private:
        F frontier;
        std::unordered_set<typename P::State> closed_list;   // contains the states of expanded nodes
    public:
        // extract problem's state and action
        using S = typename P::State;
        using A = typename P::Action;

        // Performs a search algorithm
        SearchResult<S, A> search(const P& problem) {

            SearchResult<S, A> result;
            result.goal_found = false;
            std::deque<SearchNode<S, A>> all_nodes;                                // double ended queue used to keep a pool of all allocated nodes
            all_nodes.emplace_back(problem.get_initial_state());             //  owns the node
            result.final_state = all_nodes.back().state;
            frontier.add(&all_nodes.back());                                       // observes the node

            result.metrics.generated_nodes = 1; // initial node
            result.metrics.max_frontier_size = frontier.size();

            SearchNode<S, A>* goal_node = nullptr;    // will store the goal node if any is found
            std::vector<A> executable_actions;              

            while (!frontier.empty()) {
                // expand current node
                auto const current_node = frontier.top(); frontier.pop();
                result.metrics.expanded_nodes++;
                result.metrics.max_depth = std::max(result.metrics.max_depth, current_node->depth);

                S& current_state = current_node->state;

                if(problem.is_goal(current_state)) {
                    goal_node = current_node;
                    break;
                }

                closed_list.emplace(current_state);

                // add neighbors in the frontier
                executable_actions = problem.get_actions(current_state);
                for (const auto& action : executable_actions) {

                    // generate transition
                    ailib::Transition<S, A> transition = problem.get_result(current_state, action);
                    S& child_state = transition.next_state;
                    
                    // skip closed states
                    if (closed_list.contains(child_state)) continue;

                    // add child node to the frontier
                    double child_path_cost = transition.step_cost + current_node->path_cost;

                    all_nodes.emplace_back(child_state, current_node, action, child_path_cost, current_node->depth + 1);
                    frontier.add(&all_nodes.back());
                    result.metrics.generated_nodes++;
                    result.metrics.max_depth = std::max(result.metrics.max_depth, all_nodes.back().depth);
                    result.metrics.max_frontier_size = std::max(result.metrics.max_frontier_size, frontier.size());
                }
            }

            // Retrieve found plan of actions to goal state
            if (goal_node != nullptr) {
                auto current = goal_node;
                result.goal_found = true;
                result.final_state = goal_node->state;

                while (current->parent != nullptr) {
                    result.plan.push_back(current->action);
                    current = current->parent;
                }
                std::reverse(result.plan.begin(), result.plan.end());
                
                result.score = goal_node->path_cost;
            }

            // Leave the agent reusable with a clean frontier/closed list.
            while (!frontier.empty()) {
                frontier.pop();
            }
            closed_list.clear();

            return result;
        }

        StateSearchAgent() = default;
        explicit StateSearchAgent(F f) : frontier(std::move(f)) {};

    };



    // Agent for local search problem
    template <Problem P, typename LocalPolicy> 
    class LocalSearchAgent {
    public:
        using S = typename P::State;
        using A = typename P::Action;

        template <Heuristic<S> ObjectiveFunc>
        SearchResult<S, A> search(const P& problem, ObjectiveFunc fitness) {
            SearchResult<S, A> result;
            // ... logica locale saltando da un vicino all'altro ...
            return result;
        }
    };

}