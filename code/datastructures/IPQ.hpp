#pragma once

#include <vector>
#include <unordered_map>
#include <utility>
#include "../Types.hpp"


/* 
    Indexed Priority Queue data structure
    Implemented as a minHeap allowing:
    - O(log n) insertion
    - O(log n) retrieval of the best node (lowest g value)
    - O(log n) removal of the best node
    - O(log n) update of a node's g value (decrease key operation)
    - O(1) membership test
*/

namespace ailib {
    template <typename S, typename A, typename P>
    requires(std::predicate<P, SearchNode<S, A>*, SearchNode<S, A>*>)
    class IPQ {
    private:
        using NodePtr = SearchNode<S, A>*;

        P compare_nodes;
        std::vector<NodePtr> data;
        std::unordered_map<S, std::size_t> index_map;

        void swap_nodes(std::size_t i, std::size_t j) {
            std::swap(data[i], data[j]);
            index_map[data[i]->state] = i;
            index_map[data[j]->state] = j;
        }

        void sift_up(std::size_t index) {
            while (index > 0) {
                std::size_t parent = (index - 1) / 2;
                if (compare_nodes(data[parent], data[index])) {
                    swap_nodes(parent, index);
                    index = parent;
                } else {
                    break;
                }
            }
        }

        void sift_down(std::size_t index) {
            std::size_t n = data.size();
            while (true) {
                std::size_t smallest = index;
                std::size_t left  = 2 * index + 1;
                std::size_t right = 2 * index + 2;

                if (left  < n && compare_nodes(data[smallest], data[left]))  smallest = left;
                if (right < n && compare_nodes(data[smallest], data[right])) smallest = right;

                if (smallest != index) {
                    swap_nodes(smallest, index);
                    index = smallest;
                } else {
                    break;
                }
            }
        }

    public:
        explicit IPQ(P comp_func) : compare_nodes(std::move(comp_func)) {}

        // Build heap from an existing vector in O(n)
        explicit IPQ(std::vector<NodePtr> init, P comp_func)
            : compare_nodes(std::move(comp_func)), data(std::move(init)) {
            
            for (std::size_t i = 0; i < data.size(); ++i)
                index_map[data[i]->state] = i;
                
            if (!data.empty())
                for (std::size_t i = data.size() / 2; i > 0; --i)
                    sift_down(i - 1);
        }

        void add(NodePtr node) {
            if (node == nullptr) return;

            auto it = index_map.find(node->state);
            if (it == index_map.end()) {
                index_map[node->state] = data.size();
                data.push_back(node);
                sift_up(data.size() - 1);
                return;
            }

            const std::size_t idx = it->second;
            NodePtr current = data[idx];

            if (compare_nodes(current, node)) {
                data[idx] = node;
                sift_up(idx);
            }
        }

        bool contains(const S& state) const {
            return index_map.count(state) > 0;
        }

        NodePtr top() const {
            if (data.empty()) return nullptr;
            return data.front();
        }

        void pop() {
            if (data.empty()) return;

            NodePtr best = data.front();
            index_map.erase(best->state);

            if (data.size() == 1) {
                data.pop_back();
                return;
            }

            // Move last element to root, then restore heap
            data.front() = std::move(data.back());
            data.pop_back();
            index_map[data.front()->state] = 0;
            sift_down(0);
        }

        // Call after decreasing node->path_cost to restore heap order
        void update(const S& state) {
            auto it = index_map.find(state);
            if (it == index_map.end()) return;

            std::size_t idx = it->second;
            sift_up(idx);
            sift_down(idx);
        }

        bool empty() const { return data.empty(); }
        std::size_t size() const { return data.size(); }
    };
}