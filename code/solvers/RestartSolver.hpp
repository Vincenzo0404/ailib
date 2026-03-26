#pragma once

#include "../Types.hpp"
#include "../concepts/concepts.hpp"
#include "../trackers/RestartTracker.hpp"

namespace ailib {

// RestartSolver: wraps a base solver and executes it multiple times with result
// aggregation
template <Problem P, IterativeSolverType<P> BaseSolver,
          typename AggregationPolicy =
              MaxScoreAggregation<typename BaseSolver::ResultTracker>>
class RestartSolver {
private:
  BaseSolver base_solver;
  int n_restarts;
  AggregationPolicy aggregation_policy;

public:
  using S = typename P::State;
  using BaseTracker = typename BaseSolver::ResultTracker;
  using ResultTracker = DefaultRestartTracker<BaseTracker, AggregationPolicy>;
  using AggregationPolicy_Type = AggregationPolicy; // For concept requirements

  // Constructor: takes base solver, number of restarts, and aggregation policy
  RestartSolver(BaseSolver solver, int restarts,
                AggregationPolicy policy = AggregationPolicy{})
      : base_solver(std::move(solver)), n_restarts(restarts),
        aggregation_policy(std::move(policy)) {}

  // Search method: returns aggregated results from multiple runs
  ResultTracker search(const P &problem) {
    ResultTracker tracker(aggregation_policy);

    for (int i = 0; i < n_restarts; ++i) {
      auto current_result = base_solver.search(problem);
      tracker.on_restart_result(current_result);
    }

    return tracker;
  }
};

} // namespace ailib
