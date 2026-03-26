#pragma once

#include "../Types.hpp"
#include "GeneralConcepts.hpp"

#include <concepts>

namespace ailib {

/* ===== TRACKER CONCEPTS ===== */

// Concept for trackers used by IterativeSolver
// Validates that tracker has on_start, on_step, on_finish hooks
// The hooks are templated to accept any WorkingSet type
template <typename T, typename P, typename PO>
concept IterativeTracker =
    Problem<P> && LocalPolicy<PO, P> &&
    requires(T t, typename PO::WorkingSet &ws, const PO policy) {
      { t.on_start(ws) } -> std::same_as<void>;
      { t.on_step(ws) } -> std::same_as<void>;
      { t.on_finish(ws, policy) } -> std::same_as<void>;
    };

// Concept for trackers used by ExplorationSolver
// Validates hooks for goal-based state space exploration
template <typename T, typename S, typename A>
concept ExplorationTracker =
    requires(T t, const S &state, SearchNode<S, A> *node, std::size_t size) {
      { t.on_start(state) } -> std::same_as<void>;
      { t.on_expand(node) } -> std::same_as<void>;
      { t.on_generate(node) } -> std::same_as<void>;
      { t.on_frontier_update(size) } -> std::same_as<void>;
      { t.on_finish(node) } -> std::same_as<void>;
    };

// Concept for trackers used by RestartSolver
// Validates aggregation of results from multiple runs
template <typename T, typename BT>
concept RestartTracker = requires(T t, const BT &base_result) {
  { t.on_restart_result(base_result) } -> std::same_as<void>;
};

// A policy for aggregating results in RestartSolver
// Decides how to select the "best" result from multiple iterations
template <typename AT, typename BT>
concept AggregationTracker = requires(AT agg, const BT &candidate, BT &best) {
  { agg.aggregate(candidate, best) } -> std::same_as<void>;
};

} // namespace ailib
