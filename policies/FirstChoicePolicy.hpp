#pragma once

#include "../Types.hpp"
#include "../concepts/GeneralConcepts.hpp"

#include <algorithm>
#include <random>

namespace ailib {
template <Problem P, Heuristic<typename P::State> H> class FirstChoicePolicy {
protected:
  H fitness;
  int max_lateral_moves;

private:
  using S = typename P::State;
  using A = typename P::Action;

public:
  struct WorkingSet {
    S state;
    double score = 0.0;
    int consecutive_lateral_moves = 0;
    bool hit_peak = false;
  };

  explicit FirstChoicePolicy(H f, int max_lat)
      : fitness(std::move(f)), max_lateral_moves(max_lat) {}

  void init(WorkingSet &ws, const P &problem) const {
    ws.state = problem.get_initial_state();
    ws.score = fitness(ws.state);
  }

  void step(WorkingSet &ws, const P &problem) const {

    auto actions = problem.get_actions(ws.state);

    // shuffle actions
    static thread_local std::mt19937 rng(std::random_device{}());
    std::shuffle(actions.begin(), actions.end(), rng);

    bool move_made = false;

    for (const auto &action : actions) {
      auto transition = problem.get_result(ws.state, action);
      double new_score = fitness(transition.next_state);

      // Immediately pick improvement moves
      if (new_score > ws.score) {
        ws.consecutive_lateral_moves = 0;
        ws.state = transition.next_state;
        ws.score = new_score;
        move_made = true;
        break;

        // Handle lateral moves
      } else if (new_score == ws.score) {

        if (ws.consecutive_lateral_moves >= max_lateral_moves) {
          ws.hit_peak = true;
          return;
        } else {
          ws.state = transition.next_state;
          ws.score = new_score;
          ws.consecutive_lateral_moves++;
          move_made = true;
          break;
        }
      }
    }

    // if a move hasn't been picked, then we're on a peak
    if (!move_made) {
      ws.hit_peak = true;
    }
  }

  bool is_finished(const WorkingSet &ws) const { return ws.hit_peak; }

  double get_score(const WorkingSet &ws) const { return ws.score; }

  S get_best(const WorkingSet &ws) const { return ws.state; }
};
} // namespace ailib