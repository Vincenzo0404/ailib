#include <iomanip>
#include <iostream>
#include <vector>

#include "../SolverFactory.hpp"
#include "../policies/SimulatedAnnealingPolicy.hpp"
#include "../policies/SteepestAscentPolicy.hpp"
#include "../problems/NQueens.hpp"

int main(int argc, char **argv) {
  int N;
  std::cout << "Inserisci la dimensione della scacchiera (N): ";
  std::cin >> N;

  ailib::NQueens problem(N);

  auto fitness = [&problem](const ailib::NQueens::State &state) {
    return problem.attacks_count(state);
  };

  int n_restarts = 30;

  // ===== FIRST CHOICE HILL CLIMBING =====
  std::cout << "\n" << std::string(50, '=') << "\n";
  std::cout << "1. FIRST CHOICE HILL CLIMBING\n";
  std::cout << std::string(50, '=') << "\n";

  auto fc_solver = ailib::SolverFactory::get_first_choice_hc<ailib::NQueens>(
      fitness, n_restarts, 50);

  auto fc_result = fc_solver.search(problem);
  bool fc_perfect = problem.is_goal(fc_result.best.final_state);

  std::cout << "Soluzione Perfetta: " << (fc_perfect ? "SI" : "NO") << "\n";
  std::cout << "Punteggio Finale: " << fc_result.best.score << "\n";
  std::cout << "Iterazioni: " << fc_result.total_iterations << "\n";
  std::cout << "Restart: " << fc_result.restarts_executed << "\n";
  if (fc_perfect) {
    std::cout << "Soluzione:\n";
    problem.print_state(fc_result.best.final_state);
  }

  // ===== STEEPEST ASCENT =====
  std::cout << "\n" << std::string(50, '=') << "\n";
  std::cout << "2. STEEPEST ASCENT HILL CLIMBING\n";
  std::cout << std::string(50, '=') << "\n";

  ailib::SteepestAscentPolicy<ailib::NQueens, decltype(fitness)> sa_policy(
      fitness);
  ailib::IterativeSolver<ailib::NQueens, decltype(sa_policy)> sa_base_solver(
      sa_policy, 50);

  // Wrap with RestartSolver (with 10 restarts like FirstChoice)
  auto sa_restart_solver =
      ailib::SolverFactory::create_restarting_solver<ailib::NQueens>(
          sa_policy, n_restarts);

  auto sa_result = sa_restart_solver.search(problem);
  bool sa_perfect = problem.is_goal(sa_result.best.final_state);

  std::cout << "Soluzione Perfetta: " << (sa_perfect ? "SI" : "NO") << "\n";
  std::cout << "Punteggio Finale: " << sa_result.best.score << "\n";
  std::cout << "Iterazioni Totali: " << sa_result.total_iterations << "\n";
  std::cout << "Restart: " << sa_result.restarts_executed << "\n";
  if (sa_perfect) {
    std::cout << "Soluzione:\n";
    problem.print_state(sa_result.best.final_state);
  }

  // ===== SIMULATED ANNEALING =====
  std::cout << "\n" << std::string(50, '=') << "\n";
  std::cout << "3. SIMULATED ANNEALING\n";
  std::cout << std::string(50, '=') << "\n";

  // Temperature schedule: exponential cooling
  auto cooling = [](int iteration) {
    return 100.0 * std::exp(-0.01 * iteration);
  };

  ailib::SimulatedAnnealingPolicy<ailib::NQueens, decltype(fitness),
                                  decltype(cooling)>
      sa_ann_policy(fitness, cooling);
  ailib::IterativeSolver<ailib::NQueens, decltype(sa_ann_policy)> sa_ann_solver(
      sa_ann_policy, 1000);

  auto sa_ann_result = sa_ann_solver.search(problem);
  bool sa_ann_perfect = problem.is_goal(sa_ann_result.final_state);

  std::cout << "Soluzione Perfetta: " << (sa_ann_perfect ? "SI" : "NO") << "\n";
  std::cout << "Punteggio Finale: " << sa_ann_result.score << "\n";
  std::cout << "Iterazioni: " << sa_ann_result.iterations << "\n";
  if (sa_ann_perfect) {
    std::cout << "Soluzione:\n";
    problem.print_state(sa_ann_result.final_state);
  }

  // ===== CONFRONTO =====
  std::cout << "\n" << std::string(50, '=') << "\n";
  std::cout << "CONFRONTO RISULTATI\n";
  std::cout << std::string(50, '=') << "\n";

  std::cout << std::left << std::setw(25) << "Algoritmo" << std::setw(15)
            << "Perfetto" << std::setw(15) << "Score" << std::setw(15)
            << "Iterazioni\n";
  std::cout << std::string(70, '-') << "\n";

  std::cout << std::left << std::setw(25) << "First Choice" << std::setw(15)
            << (fc_perfect ? "SI" : "NO") << std::setw(15)
            << fc_result.best.score << std::setw(15)
            << fc_result.total_iterations << "\n";

  std::cout << std::left << std::setw(25) << "Steepest Ascent" << std::setw(15)
            << (sa_perfect ? "SI" : "NO") << std::setw(15)
            << sa_result.best.score << std::setw(15)
            << sa_result.total_iterations << "\n";

  std::cout << std::left << std::setw(25) << "Simulated Annealing"
            << std::setw(15) << (sa_ann_perfect ? "SI" : "NO") << std::setw(15)
            << sa_ann_result.score << std::setw(15) << sa_ann_result.iterations
            << "\n";

  return 0;
}
