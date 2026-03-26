#pragma once

#include "Types.hpp"
#include "concepts/concepts.hpp"
#include "solvers/solvers.hpp"
#include "trackers/trackers.hpp"

#include "datastructures/FIFO.hpp"
#include "datastructures/IPQ.hpp"
#include "datastructures/LIFO.hpp"

#include "policies/FirstChoicePolicy.hpp"

namespace ailib {
class SolverFactory {
public:
  template <Problem P> static auto get_BFS() {
    return ExplorationSolver<
        P, ailib::FIFO<typename P::State, typename P::Action>>();
  }

  template <Problem P> static auto get_DFS() {
    return ExplorationSolver<
        P, ailib::LIFO<typename P::State, typename P::Action>>();
  }
  template <Problem P> static auto get_min_cost() {
    using NodePtr = SearchNode<typename P::State, typename P::Action> *;

    auto comp = [](const NodePtr a, const NodePtr b) {
      return a->path_cost > b->path_cost;
    };

    using Queue =
        ailib::IPQ<typename P::State, typename P::Action, decltype(comp)>;
    return ExplorationSolver<P, Queue>(Queue{comp});
  }

  template <Problem P, Heuristic<typename P::State> H>
  static auto get_A_star(H heuristic_func) {
    using NodePtr = SearchNode<typename P::State, typename P::Action> *;

    // this lambda function has a parameter, so it is not default-constructible
    auto comp = [heuristic_func](const NodePtr a, const NodePtr b) {
      return (a->path_cost + heuristic_func(a->state)) >
             (b->path_cost + heuristic_func(b->state));
    };

    using Queue =
        ailib::IPQ<typename P::State, typename P::Action, decltype(comp)>;

    return ExplorationSolver<P, Queue>(Queue{comp});
  }

  template <Problem P, Heuristic<typename P::State> H>
  static auto get_best_first_greedy(H heuristic_func) {
    using NodePtr = SearchNode<typename P::State, typename P::Action> *;

    auto comp = [heuristic_func](const NodePtr a, const NodePtr b) {
      return heuristic_func(a->state) > heuristic_func(b->state);
    };

    using Queue =
        ailib::IPQ<typename P::State, typename P::Action, decltype(comp)>;

    return ExplorationSolver<P, Queue>(Queue{comp});
  }

  // -- ITERATIVE IMPROVEMENT --

  template <Problem P, LocalPolicy<P> PO>
  static auto create_restarting_solver(PO policy, int n_restarts = 1) {
    using S = typename P::State;
    auto base_solver = IterativeSolver<P, PO>(policy);
    auto agg_policy = MaxScoreAggregation<DefaultIterativeTracker<P, PO>>();
    return RestartSolver<P, decltype(base_solver), decltype(agg_policy)>(
        std::move(base_solver), n_restarts, std::move(agg_policy));
  }

  template <Problem P, Heuristic<typename P::State> H>
  static auto get_first_choice_hc(H fitness, int n_restarts = 1,
                                  int max_lateral = 100) {
    ailib::FirstChoicePolicy<P, H> policy(fitness, max_lateral);
    return create_restarting_solver<P>(policy, n_restarts);
  }
};
} // namespace ailib