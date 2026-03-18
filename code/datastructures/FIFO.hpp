#pragma once

#include "../Types.hpp"
#include <queue>
#include <unordered_set>

namespace ailib {

    template <typename S, typename A>
    struct FIFO {
    
        using NodePtr = SearchNode<S, A>*;

        std::queue<NodePtr> q;
        std::unordered_set<S> present_states;

        void add(NodePtr n) { 
            q.push(n); 
            present_states.insert(n->state); 
        }
        
        NodePtr top() const { 
            return q.front(); 
        } 
        
        void pop() { 
            present_states.erase(q.front()->state);
            q.pop(); 
        }
        
        bool empty() const { 
            return q.empty(); 
        }
        
        size_t size() const { 
            return q.size(); 
        }
        
        void update(NodePtr n) { 
            if (!present_states.contains(n->state)) {
                add(n); 
            }
        }
    };

}