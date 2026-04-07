#pragma once

#include "../Types.hpp"

#include <cmath>
#include <cstddef>
#include <iostream>
#include <random>
#include <vector>

template <> struct std::hash<std::vector<int>> {
  std::size_t operator()(const std::vector<int> &v) const {
    std::size_t seed = 0;
    for (int x : v) {
      seed ^= std::hash<int>{}(x) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
};

namespace ailib {
struct NQueensAction {
  int col;
  int new_row;
};

class NQueens {
private:
  int n = 4;

public:
  using State = std::vector<int>;
  using Action = NQueensAction;

  explicit NQueens(int n) : n(n) {}

  std::vector<Action> get_actions(const State &state) const {
    std::vector<Action> actions;
    actions.reserve(static_cast<std::size_t>(n) *
                    static_cast<std::size_t>(n - 1));

    int col = 0;
    for (int q_r : state) {
      for (int i = 0; i < n; i++) {
        if (i != q_r) {
          actions.push_back(Action{col, i});
        }
      }
      ++col;
    }
    return actions;
  };

  ailib::Transition<State, Action> get_result(const State &state,
                                              const Action &action) const {
    State new_state = state;
    new_state[action.col] = action.new_row;

    Transition<State, Action> transition = {new_state, 1.0};
    return transition;
  }

  State get_initial_state() const {
    State state(n);
    // static rende il generatore persistente tra le varie chiamate
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, n - 1);

    for (int col = 0; col < n; ++col) {
      state[col] = dist(gen);
    }
    return state;
  }

  double attacks_count(const State state) const {
    int attacks = 0;

    for (int c1 = 0; c1 < n; ++c1) {
      for (int c2 = c1 + 1; c2 < n; ++c2) {
        const int r1 = state[c1];
        const int r2 = state[c2];

        const bool same_row = (r1 == r2);
        const bool same_diagonal = (std::abs(r1 - r2) == std::abs(c1 - c2));

        if (same_row || same_diagonal) {
          ++attacks;
        }
      }
    }

    return static_cast<double>(-attacks);
  }

  bool is_goal(const State &state) const { return attacks_count(state) == 0; }

  void print_state(const State &state) const {
    std::cout << "  Configurazione: [";
    for (size_t i = 0; i < state.size(); ++i) {
      if (i > 0)
        std::cout << ", ";
      std::cout << state[i];
    }
    std::cout << "]\n";
  }
};
} // namespace ailib
