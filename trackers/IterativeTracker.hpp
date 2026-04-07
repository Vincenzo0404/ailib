#pragma once

#include "../Types.hpp"
#include "../concepts/concepts.hpp"

namespace ailib {

// Default implementation of Iterative Solver tracker
template <Problem P, LocalPolicy<P> PO> struct DefaultIterativeTracker {
  // --- Result Data ---

  typename P::State final_state;
  double score = 0.0;

  // --- Metrics ---
  std::size_t iterations = 0;

  // --- Hooks (matching IterativeTracker concept) ---
  template <typename WS> void on_start(const WS &) {
    // Initialize if needed
  }

  template <typename WS> void on_step(const WS &) { iterations++; }

  template <typename WS> void on_finish(const WS &ws, PO policy) {
    final_state = policy.get_best(ws);
    score = policy.get_score(ws);
  }
};

} // namespace ailib
