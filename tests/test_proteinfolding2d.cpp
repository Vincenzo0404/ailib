#include <iomanip>
#include <iostream>
#include <string>

#include "../SolverFactory.hpp"
#include "../problems/ProteinFolding2D.hpp"

int main() {
  const std::string protein_sequence = "PHHPHPPHP";
  ailib::ProteinFolding2D problem(protein_sequence);

  std::cout << "Protein sequence: " << protein_sequence << "\n";

  auto h_max_contacts = [&problem](const ailib::ProteinFolding2D::State &s) {
    return static_cast<double>(problem.get_max_contacts(s));
  };

  auto print_result = [](const std::string &name, const auto &result) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << name << "\n";
    std::cout << std::string(60, '=') << "\n";
    std::cout << "Goal trovato: " << (result.goal_found ? "SI" : "NO") << "\n";
    std::cout << "Costo finale: " << result.score << "\n";
    std::cout << "Azioni nel piano: " << result.plan.size() << "\n";
    std::cout << "Nodi espansi: " << result.expanded_nodes << "\n";
    std::cout << "Nodi generati: " << result.generated_nodes << "\n";
    std::cout << "Max frontier size: " << result.max_frontier_size << "\n";
    std::cout << "Profondita' massima: " << result.max_depth << "\n";
  };

  auto bfs_solver = ailib::SolverFactory::get_BFS<ailib::ProteinFolding2D>();
  auto dfs_solver = ailib::SolverFactory::get_DFS<ailib::ProteinFolding2D>();
  auto min_cost_solver =
      ailib::SolverFactory::get_min_cost<ailib::ProteinFolding2D>();

  auto a_star_h1 =
      ailib::SolverFactory::get_A_star<ailib::ProteinFolding2D>(h_max_contacts);
  auto best_first_greedy =
      ailib::SolverFactory::get_best_first_greedy<ailib::ProteinFolding2D>(
          h_max_contacts);

  auto bfs_result = bfs_solver.search(problem);
  auto dfs_result = dfs_solver.search(problem);
  auto min_cost_result = min_cost_solver.search(problem);
  auto a_star_h1_result = a_star_h1.search(problem);
  auto best_first_greedy_result = best_first_greedy.search(problem);

  print_result("1. BFS", bfs_result);
  print_result("2. DFS", dfs_result);
  print_result("3. MIN COST", min_cost_result);
  print_result("4. A* con euristica get_max_contacts", a_star_h1_result);
  print_result("5. BEST FIRST GREEDY", best_first_greedy_result);

  std::cout << "\n" << std::string(60, '=') << "\n";
  std::cout << std::left << std::setw(44) << "Algoritmo" << std::setw(8)
            << "Goal" << std::setw(12) << "Costo" << "Espansi\n";
  std::cout << std::string(60, '-') << "\n";

  std::cout << std::left << std::setw(44) << "BFS" << std::setw(8)
            << (bfs_result.goal_found ? "SI" : "NO") << std::setw(12)
            << bfs_result.score << bfs_result.expanded_nodes << "\n";

  std::cout << std::left << std::setw(44) << "DFS" << std::setw(8)
            << (dfs_result.goal_found ? "SI" : "NO") << std::setw(12)
            << dfs_result.score << dfs_result.expanded_nodes << "\n";

  std::cout << std::left << std::setw(44) << "MIN COST" << std::setw(8)
            << (min_cost_result.goal_found ? "SI" : "NO") << std::setw(12)
            << min_cost_result.score << min_cost_result.expanded_nodes << "\n";

  std::cout << std::left << std::setw(44) << "A* get_max_contacts"
            << std::setw(8) << (a_star_h1_result.goal_found ? "SI" : "NO")
            << std::setw(12) << a_star_h1_result.score
            << a_star_h1_result.expanded_nodes << "\n";

  std::cout << std::left << std::setw(44) << "BEST FIRST GREEDY" << std::setw(8)
            << (best_first_greedy_result.goal_found ? "SI" : "NO")
            << std::setw(12) << best_first_greedy_result.score
            << best_first_greedy_result.expanded_nodes << "\n";

  return 0;
}
