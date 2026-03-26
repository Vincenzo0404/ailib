# AIlib - C++20 Solver Framework

C++20 template-based library for implementing and benchmarking GOFAI algorithms. Uses concept constraints for compile-time type checking and zero-overhead abstractions.

## Structure

```
concepts/      Type contracts enforcing interface requirements
policies/      Algorithm implementations (hill-climbing variants)
solvers/       Orchestration layer coordinating policies with problems
trackers/      Monitoring hooks at key solver phases
problems/      Domain implementations (N-Queens, 8-Puzzle)
datastructures/ Container types (queues, stacks, priority queues)
tests/         Benchmarks comparing algorithms on different problems
```

## Quick Start

```bash
make all       # Compile all
make test_nq   # Run N-Queens benchmark
make test_p8   # Run Puzzle8 benchmark
```

## Key Points

- Template specialization for algorithm variants (no inheritance)
- Policies define own working set structure
- Add new algorithms: implement single policy class only
- Trackers provide hooks for solvers' performance and metrics detection

## Requirements

C++20 compiler
