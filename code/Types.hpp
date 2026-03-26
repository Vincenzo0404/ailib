#pragma once

#include <algorithm>
#include <concepts>
#include <vector>

namespace ailib {

    // Represent a transition within a search state exploration problem
    template <typename S, typename A>
    struct Transition {
        S next_state;
        double step_cost;
    };

    template <typename S, typename A>
    struct SearchNode {
        S state;
        SearchNode* parent;  //not using shared_ptr to avoid recursive delete and segfault
        A action;
        double path_cost;
        int depth;

        SearchNode(S s, SearchNode* p = nullptr, A a = {}, double c = 0.0, int d = 0)
            : state(s), parent(p), action(a), path_cost(c), depth(d) {}
    };
}