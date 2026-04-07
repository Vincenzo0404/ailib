#pragma once

#include "../Types.hpp"
#include <array>
#include <cstddef>
#include <functional>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace ailib {

class ProteinFolding2D {
private:
  int n;
  std::string protein;
  std::vector<int> max_h_future; // represents the maximum possible number of
                                 // contacts for each aminoacid placed

public:
  static std::string random_protein(int length) {
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_int_distribution<int> bit(0, 1);
    std::string s(static_cast<std::size_t>(length), 'P');
    for (char &c : s)
      c = bit(gen) ? 'H' : 'P';
    return s;
  }

  struct State {
    int aminoacids_used;
    int n_contacts;
    std::pair<int, int> last_idx;
    std::vector<char> grid;

    bool operator==(const State &) const = default;
  };

  enum class Action { UP, DOWN, LEFT, RIGHT };

  static constexpr std::array<std::pair<int, int>, 4> ACTION_DELTAS = {
      std::pair<int, int>{0, 1},  // UP
      std::pair<int, int>{0, -1}, // DOWN
      std::pair<int, int>{-1, 0}, // LEFT
      std::pair<int, int>{1, 0}   // RIGHT
  };

  static constexpr std::array<Action, 4> ALL_ACTIONS = {
      Action::UP, Action::DOWN, Action::LEFT, Action::RIGHT};

  static constexpr std::pair<int, int> action_delta(Action action) {
    return ACTION_DELTAS[static_cast<std::size_t>(action)];
  }

  explicit ProteinFolding2D(int size)
      : ProteinFolding2D(random_protein(size)) {}

  ProteinFolding2D(std::string protein_sequence)
      : protein(std::move(protein_sequence)) {
    n = static_cast<int>(protein.size());
    if (n == 0)
      throw std::invalid_argument("ProteinFolding2D: string must be non-null");
    max_h_future.resize(n);

    int current_potential = 0;
    for (int i = n - 1; i >= 0; --i) {
      if (protein[i] == 'H') {
        if (i == n - 1)
          current_potential += 3; // last H
        else
          current_potential += 2; // internal H
      }
      max_h_future[i] = current_potential;
    }
  }

  bool in_bounds(int x, int y) const {
    return x >= 0 && y >= 0 && x < n && y < n;
  }

  int to_index(int x, int y) const { return y * n + x; }

  std::vector<Action> get_actions(State s) const {
    std::vector<Action> actions;
    if (s.last_idx.first > 0)
      actions.push_back(Action::LEFT);
    if (s.last_idx.second > 0)
      actions.push_back(Action::DOWN);
    if (s.last_idx.first < n - 1)
      actions.push_back(Action::RIGHT);
    if (s.last_idx.second < n - 1)
      actions.push_back(Action::UP);
    return actions;
  }

  Transition<State, Action> get_result(State s, const Action &a) const {
    Transition<State, Action> transition;

    s.aminoacids_used += 1;
    char next_char = protein.at(s.aminoacids_used);

    auto delta = action_delta(a);

    // computing new index
    s.last_idx.first += delta.first;
    s.last_idx.second += delta.second;

    // place new aminoacid
    const int x = s.last_idx.first;
    const int y = s.last_idx.second;
    s.grid[to_index(x, y)] = next_char;

    int new_contacts = 0; // tracks new contacts

    const auto opp = static_cast<Action>(static_cast<unsigned>(a) ^ 1u);
    for (Action d : ALL_ACTIONS) {
      if (d != opp) {
        const auto neigh_delta = action_delta(d);
        const int nx = x + neigh_delta.first;
        const int ny = y + neigh_delta.second;

        if (!in_bounds(nx, ny))
          continue;

        char neighbor_cell = s.grid[to_index(nx, ny)];

        if (neighbor_cell == 'H' && next_char == 'H') {
          new_contacts++;
        }
      }
    }

    s.n_contacts += new_contacts;

    transition.next_state = s;
    transition.step_cost =
        3 - new_contacts; // shift weights by 3 to obtain UCS optimality having
                          // non-negative weights

    return transition;
  }

  State get_initial_state() const {
    State s;
    s.grid.assign(static_cast<std::size_t>(n * n), 'N');
    s.last_idx = {n / 2, n / 2};
    s.aminoacids_used = 0;
    s.n_contacts = 0;
    s.grid[to_index(s.last_idx.first, s.last_idx.second)] = protein.front();

    return s;
  }

  bool is_goal(const State &s) const { return s.aminoacids_used == n - 1; }

  /* HEURISTICS */

  int get_max_contacts(const State &s) const {
    const int remaining_steps = (n - 1) - s.aminoacids_used;
    return (3 * remaining_steps) - max_h_future[s.aminoacids_used];
  }
};

} // namespace ailib

template <> struct std::hash<ailib::ProteinFolding2D::State> {
  std::size_t operator()(const ailib::ProteinFolding2D::State &s) const {
    std::size_t seed = 0;
    auto hash_combine = [&seed](std::size_t value) {
      seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    };

    hash_combine(std::hash<int>{}(s.aminoacids_used));
    hash_combine(std::hash<int>{}(s.n_contacts));
    hash_combine(std::hash<int>{}(s.last_idx.first));
    hash_combine(std::hash<int>{}(s.last_idx.second));
    for (char c : s.grid)
      hash_combine(std::hash<char>{}(c));

    return seed;
  }
};