#pragma once

#include <concepts>
#include <functional>
#include <vector>

namespace ailib {

    // Data structure used for algorithm profiling
    struct SearchMetrics {
        size_t expanded_nodes = 0;     
        size_t generated_nodes = 0;     
        size_t max_frontier_size = 0;   
        int max_depth = 0;  
    };

    // Generic result for a search algorithm
    template <typename S, typename A>
    struct SearchResult {
        bool goal_found = false;       // Indica se ha effettivamente trovato il goal
        S final_state;                 
        double score = 0.0;            
        std::vector<A> plan;           
        SearchMetrics metrics;         // <--- Le nostre nuove metriche!
    };

    // Represent a transition within a search state exploration problem
    template <typename S, typename A>
    struct Transition {
        S next_state;
        double step_cost;
    };

    template <typename S, typename A>
    struct SearchNode {
        S state;
        SearchNode* parent;  //not using shared_ptr to avoid recursive delete and segfault
        A action;
        double path_cost;
        int depth;

        SearchNode(S s, SearchNode* p = nullptr, A a = {}, double c = 0.0, int d = 0)
            : state(s), parent(p), action(a), path_cost(c), depth(d) {}
    };


    /*-- CONCEPTS --*/

    template<typename S>
    concept State = std::equality_comparable<S> && requires(const S state) {
        {std::hash<S>{}(state) } -> std::same_as<size_t>;   //the state must be hashable
    };

    template<typename N, typename A>
    concept Node = requires(const N node) {
        { node.fValue() }   -> std::convertible_to<double>;
        { node.parent }     -> std::same_as<const N*>;
        { node.action }     -> std::convertible_to<A>; //TODO  questa A va legata nelle possibili azioni dell'agent
    };

    template <typename H, typename S>
    concept Heuristic = State<S> && requires(const H heuristic, const S state) {
        { heuristic(state) } -> std::convertible_to<double>;
    };

    template<typename P>
    concept Problem = requires {
        // A Problem must declare a State and an Action
        typename P::State; 
        typename P::Action;
    } 
    && State<typename P::State> 
    && requires(const P problem, const typename P::State from, const typename P::Action action) {
        { problem.get_initial_state() }      -> std::convertible_to<typename P::State>;
        { problem.get_actions(from) }        -> std::same_as<std::vector<typename P::Action>>;
        { problem.get_result(from, action) } -> std::convertible_to<Transition<typename P::State, typename P::Action>>;
        { problem.is_goal(from) }            -> std::same_as<bool>;
    };

    template <typename Ag, typename P>
    concept Agent = Problem<P> && requires(Ag agent, const P problem) {
        { agent.search(problem) } -> std::convertible_to< SearchResult<typename P::State, typename P::Action> >;
    };

    template <typename Q, typename N>
    concept Frontier= requires(Q q, const Q cq, N* n) {
        { q.add(n) } -> std::same_as<void>;     // same_as is a std concept which requires exact match in type
        { q.top() } -> std::same_as<N*>;   // e.g. requires that the frontier.top() returns exactly a NodePtr
        { q.pop() } -> std::same_as<void>;
        { cq.empty() } -> std::convertible_to<bool>;        // cq is used to express concept clauses by which the queue must
        { cq.size() } -> std::convertible_to<std::size_t>;  // be considered constant (e.g. in empty and size methods there must be 
    };                                                      // no side effects)


};