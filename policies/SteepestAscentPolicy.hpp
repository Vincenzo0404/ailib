#pragma once

#include "../concepts/GeneralConcepts.hpp"
#include "FirstChoicePolicy.hpp"

namespace ailib {

template <Problem P, Heuristic<typename P::State> H>
class SteepestAscentPolicy : public FirstChoicePolicy<P, H> {
public:
  // Custom constructor: SteepestAscent doesn't need max_lateral_moves
  explicit SteepestAscentPolicy(H f)
      : FirstChoicePolicy<P, H>(f, 0) {
  } // Pass 0 for max_lateral_moves (unused)

  void step(typename FirstChoicePolicy<P, H>::WorkingSet &ws,
            const P &problem) const {

    auto actions = problem.get_actions(ws.state);

    double best_score = ws.score;
    auto best_state = ws.state;
    bool improvement_found = false;

    // Evaluate all actions
    for (const auto &action : actions) {
      auto transition = problem.get_result(ws.state, action);
      double new_score = this->fitness(transition.next_state);

      if (new_score > best_score) {
        best_score = new_score;
        best_state = transition.next_state;
        improvement_found = true;
      }
    }

    if (improvement_found) {
      ws.state = best_state;
      ws.score = best_score;
    } else {
      ws.hit_peak = true;
    }
  }
};
} // namespace ailib