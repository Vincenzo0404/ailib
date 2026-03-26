#pragma once

#include "../Types.hpp"
#include "../concepts/concepts.hpp"
#include "../trackers/IterativeTracker.hpp"

namespace ailib {

// Solver for iterative improvement / local search problems
template <Problem P, LocalPolicy<P> PO> class IterativeSolver {
private:
  PO policy;
  int max_iterations;

public:
  using ResultTracker =
      DefaultIterativeTracker<P, PO>; // Declare result tracker type

  explicit IterativeSolver(PO p, int max_iterations = 10000000)
      : policy(p), max_iterations(max_iterations) {};

  // Search method with tracker that conforms to IterativeTracker concept
  template <IterativeTracker<P, PO> Tracker = ResultTracker>
  Tracker search(const P &problem, Tracker tracker = Tracker{}) {

    typename PO::WorkingSet ws;

    policy.init(ws, problem);

    tracker.on_start(ws);

    int iterations = 0;

    while (!policy.is_finished(ws) && iterations < max_iterations) {
      policy.step(ws, problem);
      tracker.on_step(ws);
      iterations++;
    }

    tracker.on_finish(ws, policy);

    return tracker;
  }
};

} // namespace ailib
