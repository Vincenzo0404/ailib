#pragma once

#include "../Types.hpp"
#include "../concepts/GeneralConcepts.hpp"
#include "FirstChoicePolicy.hpp"

#include <cmath>
#include <concepts>
#include <random>

namespace ailib {
template <Problem P, Heuristic<typename P::State> H,
          typename TemperatureSchedule>
  requires requires(TemperatureSchedule sched, int iteration) {
    { sched(iteration) } -> std::convertible_to<double>;
  }
class SimulatedAnnealingPolicy {
  using S = P::State;
  using A = P::Action;

  std::mt19937 rng{std::random_device{}()};
  H fitness;
  TemperatureSchedule temperature_schedule;
  std::uniform_real_distribution<double> rdis{0.0, 1.0};

public:
  struct WorkingSet {
    S state;
    int current_iteration = 0;
    double score = 0;
    double last_temperature;
  };

  void init(WorkingSet &ws, const P &problem) const {
    ws.state = problem.get_initial_state();
    ws.score = fitness(ws.state);
    ws.current_iteration = 0;
    ws.last_temperature = temperature_schedule(0);
  }

  void step(WorkingSet &ws, const P &problem) {

    // get current temperature
    double temperature = temperature_schedule(ws.current_iteration);
    ws.current_iteration++;
    ws.last_temperature = temperature;
    if (temperature == 0)
      return;

    // pick a random move
    auto actions = problem.get_actions(ws.state);
    std::uniform_int_distribution<> dis(0, actions.size() - 1);
    auto random_action = actions[dis(rng)];

    Transition<S, A> transition = problem.get_result(ws.state, random_action);

    double new_score = fitness(transition.next_state);

    // always choose improving moves
    if (new_score > ws.score) {
      ws.state = transition.next_state;
      ws.score = new_score;
      return;
    }

    // consider pejorative moves based on decreasing probability over time
    double delta = new_score - ws.score;
    double p_accept = std::exp(delta / temperature);

    if (rdis(rng) < p_accept) {
      ws.state = transition.next_state;
      ws.score = new_score;
    }
  }

  bool is_finished(const WorkingSet &ws) const {
    return ws.last_temperature == 0;
  }

  double get_score(const WorkingSet &ws) const { return ws.score; }

  S get_best(const WorkingSet &ws) const { return ws.state; }

  explicit SimulatedAnnealingPolicy(H h, TemperatureSchedule ts)
      : fitness(h), temperature_schedule(ts) {}
};
} // namespace ailib