#include <iostream>
#include "Types.hpp"
#include "AgentFactory.hpp"
#include "problems/puzzle8.hpp"

using namespace ailib;

static void print_result_summary(const char* label, const SearchResult<Puzzle8::State, Puzzle8::Action>& result) {
    std::cout << "\n=== " << label << " ===\n";
    std::cout << "Goal found: " << (result.goal_found ? "yes" : "no") << "\n";
    std::cout << "Score: " << result.score << "\n";
    std::cout << "Plan length: " << result.plan.size() << "\n";
    std::cout << "Expanded nodes: " << result.metrics.expanded_nodes << "\n";
    std::cout << "Generated nodes: " << result.metrics.generated_nodes << "\n";
    std::cout << "Max frontier size: " << result.metrics.max_frontier_size << "\n";
    std::cout << "Max depth: " << result.metrics.max_depth << "\n";
}

int main() {
    Puzzle8 problem;
    std::cout << "Initial state:\n" << problem.to_string(problem.get_initial_state()) << std::endl;

    // 1. Blind Search
    auto bfs_agent = AgentFactory::getBFS<Puzzle8>();
    auto dfs_agent = AgentFactory::getDFS<Puzzle8>();

    // 2. Informed Search (Passiamo le euristiche come parametri!)
    auto astar_misplaced = AgentFactory::getAStar<Puzzle8>(Puzzle8::misplaced_tiles);
    auto astar_manhattan = AgentFactory::getAStar<Puzzle8>(Puzzle8::manhattan_distance);

    // Esecuzione
    auto bfs_result = bfs_agent.search(problem);
    auto dfs_result = dfs_agent.search(problem);
    auto astar_m_result = astar_misplaced.search(problem);
    auto astar_md_result = astar_manhattan.search(problem);

    // Stampa risultati
    print_result_summary("BFS (Cieca)", bfs_result);
    print_result_summary("DFS (Cieca, Sub-Ottima)", dfs_result);
    print_result_summary("A* (Euristica: Tessere fuori posto)", astar_m_result);
    print_result_summary("A* (Euristica: Distanza Manhattan)", astar_md_result);

    return 0;
}