#pragma once

#include "../Types.hpp"
#include <array>
#include <iostream>
#include <string>

// Hash specialization for Puzzle8::State (std::array<int, 9>)
template <> struct std::hash<std::array<int, 9>> {
  size_t operator()(const std::array<int, 9> &arr) const {
    size_t seed = 0;
    for (int v : arr)
      seed ^= std::hash<int>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
  }
};

namespace ailib {
class Puzzle8 {
public:
  using State =
      std::array<int, 9>; // flat 3x3 grid representing the puzzle state
  enum class Action { UP, DOWN, LEFT, RIGHT }; // possible moves

  static State get_initial_state() { return {1, 5, 3, 8, 6, 0, 7, 2, 4}; }

  static std::vector<Action> get_actions(const State &state) {
    std::vector<Action> actions;
    int zero_idx = 0;
    for (int i = 0; i < 9; ++i)
      if (state[i] == 0) {
        zero_idx = i;
        break;
      }

    int zero_row = zero_idx / 3;
    int zero_col = zero_idx % 3;

    if (zero_row > 0)
      actions.push_back(Action::UP);
    if (zero_row < 2)
      actions.push_back(Action::DOWN);
    if (zero_col > 0)
      actions.push_back(Action::LEFT);
    if (zero_col < 2)
      actions.push_back(Action::RIGHT);

    return actions;
  }

  static Transition<State, Action> get_result(const State &state,
                                              const Action action) {
    State new_state = state;
    int zero_idx = 0;
    for (int i = 0; i < 9; ++i)
      if (state[i] == 0) {
        zero_idx = i;
        break;
      }

    int swap_idx = zero_idx;
    switch (action) {
    case Action::UP:
      swap_idx = zero_idx - 3;
      break;
    case Action::DOWN:
      swap_idx = zero_idx + 3;
      break;
    case Action::LEFT:
      swap_idx = zero_idx - 1;
      break;
    case Action::RIGHT:
      swap_idx = zero_idx + 1;
      break;
    }
    std::swap(new_state[zero_idx], new_state[swap_idx]);

    return {new_state, 1.0};
  }

  static bool is_goal(const State &state) {
    return state == State{1, 2, 3, 4, 5, 6, 7, 8, 0};
  }

  static std::string action_to_string(Action action) {
    switch (action) {
    case Action::UP:
      return "UP";
    case Action::DOWN:
      return "DOWN";
    case Action::LEFT:
      return "LEFT";
    case Action::RIGHT:
      return "RIGHT";
    }
    return "UNKNOWN";
  }

  static std::string to_string(const State &state) {
    std::string result;
    for (int i = 0; i < 9; ++i) {
      result += std::to_string(state[i]) + " ";
      if ((i + 1) % 3 == 0)
        result += "\n";
    }
    return result;
  }

  static void print_state(const State &state) {
    std::cout << "  Configurazione:\n" << to_string(state);
  }

  static double misplaced_tiles(const State &state) {
    double count = 0.0;
    for (int i = 0; i < 9; ++i) {
      int tile = state[i];
      if (tile != 0) {
        int target_val = (i == 8) ? 0 : (i + 1);
        if (tile != target_val) {
          count += 1.0;
        }
      }
    }
    return count;
  }

  static double manhattan_distance(const State &state) {
    double distance = 0.0;
    for (int i = 0; i < 9; ++i) {
      int tile = state[i];
      if (tile != 0) {
        int target_idx = tile - 1;

        int target_row = target_idx / 3;
        int target_col = target_idx % 3;

        int current_row = i / 3;
        int current_col = i % 3;

        distance += std::abs(target_row - current_row) +
                    std::abs(target_col - current_col);
      }
    }
    return distance;
  }
};
} // namespace ailib