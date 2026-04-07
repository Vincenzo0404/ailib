#pragma once

#include "../Types.hpp"
#include <concepts>
#include <functional>
#include <vector>

namespace ailib {
template <typename S>
concept State = std::equality_comparable<S> && requires(const S state) {
  {
    std::hash<S>{}(state)
  } -> std::same_as<size_t>; // the state must be hashable
};

template <typename H, typename S>
concept Heuristic = State<S> && requires(const H heuristic, const S state) {
  { heuristic(state) } -> std::convertible_to<double>;
};

template <typename P>
concept Problem =
    requires {
      // A Problem must declare a State and an Action type
      typename P::State;
      typename P::Action;
    } && State<typename P::State> &&
    requires(const P problem, const typename P::State from,
             const typename P::Action action) {
      {
        problem.get_actions(from)
      } -> std::same_as<std::vector<typename P::Action>>;
      {
        problem.get_result(from, action)
      }
      -> std::convertible_to<Transition<typename P::State, typename P::Action>>;
      { problem.get_initial_state() } -> std::convertible_to<typename P::State>;
    };

template <typename GP>
concept GoalProblem =
    Problem<GP> && requires(const GP problem, const typename GP::State state) {
      { problem.is_goal(state) } -> std::same_as<bool>;
    };

template <typename Q, typename N>
concept Frontier = requires(Q q, const Q cq, N *n) {
  { q.add(n) } -> std::same_as<void>;
  { q.top() } -> std::same_as<N *>;
  { q.pop() } -> std::same_as<void>;
  { cq.empty() } -> std::convertible_to<bool>;
  { cq.size() } -> std::convertible_to<std::size_t>;
};

template <typename PO, typename P>
concept LocalPolicy =
    Problem<P> && requires(PO policy, typename PO::WorkingSet &working_set,
                           const P &problem) {
      // Policy must define its working set type
      typename PO::WorkingSet;

      // policy needs the problem's instance to know available actions from a
      // certain state
      { policy.init(working_set, problem) } -> std::same_as<void>;
      { policy.step(working_set, problem) } -> std::same_as<void>;

      // exit condition
      { policy.is_finished(working_set) } -> std::convertible_to<bool>;

      // extracts best state from working set
      { policy.get_best(working_set) } -> std::same_as<typename P::State>;

      // extracts score from working set
      { policy.get_score(working_set) } -> std::convertible_to<double>;
    };
} // namespace ailib
