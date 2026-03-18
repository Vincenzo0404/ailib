#pragma once

#include "../Types.hpp"
#include <stack>
#include <unordered_set>

namespace ailib {

    template<typename S, typename A>
    struct LIFO {
        using NodePtr = SearchNode<S, A>*;

        std::stack<NodePtr> stack;
        std::unordered_set<S> present_states;

        void add(NodePtr n) {
            if (!present_states.contains(n->state)) {
                stack.push(n); 
                present_states.insert(n->state); 
            }
        }
        
        NodePtr top() const { 
            return stack.top(); 
        } 
        
        void pop() { 
            present_states.erase(stack.top()->state);
            stack.pop(); 
        }
        
        bool empty() const { 
            return stack.empty(); 
        }
        
        size_t size() const { 
            return stack.size(); 
        }
    
    };

}
