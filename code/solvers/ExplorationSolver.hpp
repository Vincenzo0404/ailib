#pragma once

#include "../Types.hpp"
#include "../concepts/concepts.hpp"
#include "../trackers/ExplorationTracker.hpp"
#include <algorithm>
#include <deque>
#include <unordered_set>
#include <vector>

namespace ailib {

// Solver for goal-based state space exploration problems
template <GoalProblem P,
          Frontier<SearchNode<typename P::State, typename P::Action>> F>
class ExplorationSolver {
private:
  F frontier;
  std::unordered_set<typename P::State> closed_list;

public:
  using S = typename P::State;
  using A = typename P::Action;
  using ResultTracker =
      DefaultExplorationTracker<S, A>; // Declare result tracker type

  // Performs a goal-based search algorithm
  template <typename Tracker = ResultTracker>
    requires ExplorationTracker<Tracker, S, A>
  Tracker search(const P &problem, Tracker tracker = Tracker{}) {

    std::deque<SearchNode<S, A>> all_nodes;
    all_nodes.emplace_back(problem.get_initial_state());
    frontier.add(&all_nodes.back());

    tracker.on_start(all_nodes.back().state);
    tracker.on_generate(&all_nodes.back());
    tracker.on_frontier_update(frontier.size());

    SearchNode<S, A> *goal_node = nullptr;
    std::vector<A> executable_actions;

    while (!frontier.empty()) {
      auto const current_node = frontier.top();
      frontier.pop();
      tracker.on_expand(current_node);

      S &current_state = current_node->state;

      if (problem.is_goal(current_state)) {
        goal_node = current_node;
        break;
      }

      closed_list.emplace(current_state);

      executable_actions = problem.get_actions(current_state);
      for (const auto &action : executable_actions) {

        ailib::Transition<S, A> transition =
            problem.get_result(current_state, action);
        S &child_state = transition.next_state;

        if (closed_list.contains(child_state))
          continue;

        double child_path_cost = transition.step_cost + current_node->path_cost;

        all_nodes.emplace_back(child_state, current_node, action,
                               child_path_cost, current_node->depth + 1);
        frontier.add(&all_nodes.back());
        tracker.on_generate(&all_nodes.back());
        tracker.on_frontier_update(frontier.size());
      }
    }

    tracker.on_finish(goal_node);

    // Clean up for reusability
    while (!frontier.empty()) {
      frontier.pop();
    }
    closed_list.clear();

    return tracker;
  }

  ExplorationSolver() = default;
  explicit ExplorationSolver(F f) : frontier(std::move(f)) {};
};

} // namespace ailib
