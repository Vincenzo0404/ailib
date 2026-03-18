#pragma once

#include "Types.hpp"
#include <chrono>
#include <vector>
#include <omp.h>

namespace ailib {

    struct AgentMetrics {
        bool success;               // Ha trovato il goal?
        double execution_time_ms;   // Tempo di esecuzione in millisecondi
        double cost;                // Costo del percorso (result.score)
        size_t plan_length;         // Numero di azioni nel piano
        size_t frontier_max_size;
        size_t max_depth;
        
    };

    class Evaluator {
    private:
        // 2. Metodo helper privato che valuta UN SINGOLO agente
        template <Problem P, Agent<P> Ag>
        static AgentMetrics evaluate_single(Ag& agent, const P& problem) {
            // Facciamo partire il cronometro
            auto start = std::chrono::high_resolution_clock::now();
            
            // Eseguiamo la ricerca
            auto result = agent.search(problem);
            
            // Fermiamo il cronometro
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> duration = end - start;

            // Restituiamo le metriche
            return AgentMetrics {
                !result.plan.empty(),   // success: assumiamo vero se il piano non è vuoto
                duration.count(),       // tempo in millisecondi
                result.score,           // costo
                result.plan.size()      // lunghezza del piano
            };
        }

    public:
        // 3. Metodo pubblico magico che accetta N agenti diversi!
        // I "..." indicano che Ags è un "pacchetto" (pack) di tipi diversi
        template <Problem P, Agent<P>... Ags>
        static std::vector<AgentMetrics> compare(const P& problem, Ags&... agents) {
            
            // Questa sintassi (introdotta in C++17) applica evaluate_single a ogni 
            // agente nel pacchetto e infila i risultati direttamente in un vector!
            return { evaluate_single(agents, problem)... };
        }
    };

}