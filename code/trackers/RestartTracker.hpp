#pragma once

#include "../Types.hpp"
#include "../concepts/concepts.hpp"

namespace ailib {

// Default aggregation policy for RestartSolver: maximize score
template <typename BT> class MaxScoreAggregation {
public:
  void aggregate(const BT &candidate, BT &best) const {
    if (candidate.score > best.score) {
      best = candidate;
    }
  }
};

// Generic RestartTracker with customizable aggregation policy
template <typename BT, typename AP = MaxScoreAggregation<BT>>
struct DefaultRestartTracker {
  BT best;
  std::size_t restarts_executed = 0;
  std::size_t total_iterations = 0;
  AP aggregation_policy;
  bool has_result = false;

  // Default constructor
  DefaultRestartTracker() = default;

  // Constructor taking aggregation policy
  explicit DefaultRestartTracker(AP policy)
      : aggregation_policy(std::move(policy)) {}

  // Hook: called when a restart iteration completes
  void on_restart_result(const BT &current) {
    ++restarts_executed;

    // Aggregate iterations if the tracker has them
    if constexpr (requires { current.iterations; }) {
      total_iterations += current.iterations;
    }

    // Use aggregation policy to update best result
    if (!has_result) {
      best = current;
      has_result = true;
    } else {
      aggregation_policy.aggregate(current, best);
    }
  }
};

} // namespace ailib
