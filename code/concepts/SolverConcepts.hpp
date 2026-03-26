#pragma once

#include "../Types.hpp"
#include "GeneralConcepts.hpp"

namespace ailib {

/* ===== BASE SOLVER CONCEPT ===== */

// A Solver must implement search(problem) returning some tracker
template <typename SV, typename P>
concept Solver = Problem<P> && requires(SV solver, const P problem) {
  { solver.search(problem) };
};

/* ===== SOLVER TYPE SPECIALIZATIONS ===== */

// IterativeSolver must have ResultTracker that conforms to IterativeTracker
// concept Note: The ResultTracker type is what the solver's search() method
// returns
template <typename SV, typename P>
concept IterativeSolverType =
    Solver<SV, P> && requires { typename SV::ResultTracker; };
// The actual tracker concept validation is deferred to the search method

// ExplorationSolver must have ResultTracker for goal-based exploration
template <typename SV, typename P>
concept ExplorationSolverType =
    Solver<SV, P> && requires { typename SV::ResultTracker; };
// The actual tracker concept validation is deferred to the search method

// RestartSolver wraps another solver and must have AggregationPolicy
template <typename SV, typename P>
concept RestartSolverType =
    Solver<SV, P> && requires { typename SV::AggregationPolicy; };

} // namespace ailib
