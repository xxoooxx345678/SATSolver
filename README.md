# SAT Solver

[SAT](https://en.wikipedia.org/wiki/Boolean_satisfiability_problem) is the first problem that was proven to be NP-complete, and generally is believed that no algorithm efficiently solves each SAT problem. SAT has many applications in real life, e.g. artificial intelligence, circuit design, and automatic theorem proving.  
SAT Solver is aim to solve the SAT problem, it takes clauses in CNF as input and tries to solve them and output the value of each literal such that satisfy the clauses.  

## Usage

#### Change directory
```
$ cd satsolver
```

#### Compile
```
$ make [NIVER | RESTART]
```

#### Execute

<code>
$ ./sat <i>input-file</i></code>


#### clean up
```
$ make clean
```

## Heuristics & Implementations

- [x] Backtracking-based search algorithm (DPLL)
- [x] Non-chronological backtracking (CDCL)
- [x] Effective preprocessing (NiVER)
- [x] Parallelization
- [x] Random restart
- [x] Optimized priority queue (Heap)

## TODO

- [ ] Verifier
- [ ] Better Parallelization
- [ ] Reduce Memory Usage

