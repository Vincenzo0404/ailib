#pragma once

#include "Types.hpp"
#include "DefaultAgents.hpp"
#include "datastructures/FIFO.hpp"
#include "datastructures/LIFO.hpp"
#include "datastructures/IPQ.hpp"

namespace ailib {
    class AgentFactory {
    public:
    
        template<Problem P>
        static auto getBFS() {
            return StateSearchAgent<P, ailib::FIFO<typename P::State, typename P::Action>>();
        }

        template<Problem P>
        static auto getDFS() {
            return StateSearchAgent<P, ailib::LIFO<typename P::State, typename P::Action>>();
        }
        template<Problem P>
        static auto getMinCost() {
            using NodePtr = SearchNode<typename P::State, typename P::Action>*;
            
            auto comp = [](const NodePtr a, const NodePtr b) {
                return a->path_cost > b->path_cost;
            };
            
            using Queue = ailib::IPQ<typename P::State, typename P::Action, decltype(comp)>;
            return StateSearchAgent<P, Queue>(Queue{comp}); 
        }

        template<Problem P, Heuristic<typename P::State> H>
        static auto getAStar(H heuristic_func) {
            using NodePtr = SearchNode<typename P::State, typename P::Action>*;
            
            // Questa lambda CATTURA l'euristica, quindi non è default-constructible
            auto comp = [heuristic_func](const NodePtr a, const NodePtr b) {
                return (a->path_cost + heuristic_func(a->state)) > (b->path_cost + heuristic_func(b->state));
            };
            
            using Queue = ailib::IPQ<typename P::State, typename P::Action, decltype(comp)>;
            
            // Passiamo la coda configurata all'agente
            return StateSearchAgent<P, Queue>(Queue{comp}); 
        }

        template<Problem P, Heuristic<typename P::State> H>
        static auto getBestFirstGreedy(H heuristic_func) {
            using NodePtr = SearchNode<typename P::State, typename P::Action>*;
            
            auto comp = [heuristic_func](const NodePtr a, const NodePtr b) {
                return heuristic_func(a->state) > heuristic_func(b->state);
            };
            
            using Queue = ailib::IPQ<typename P::State, typename P::Action, decltype(comp)>;
            
            return StateSearchAgent<P, Queue>(Queue{comp}); 
        }
    };
}