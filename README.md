# subgoal-mcts

Partial code for Master's thesis *Subgoal-Based Temporal Abstractions in Monte-Carlo Tree Search* (2018) by Jan Peter.

---

To address the problem of efficiently generating macro-actions from a subgoal predicate in deterministic domains, we propose an extension to *Monte-Carlo Tree Search (MCTS)* that dynamically samples an approximate set of available macro-actions locally for each state. This is done by modifying the expansion phase of MCTS to generate random sequences of primitive actions until a subgoal is reached. Once the likelihood of discovering further macro-actions that terminate in a distinct subgoal drops below a threshold, the set of macro-actions is considered complete and is used for further planning. By treating this macro-action sampling as a Bernoulli trial, we do not make any assumptions about the number of macro-actions possible in each state and only rely on a desired *action coverage* along with a tolerated error.

This method exploits the fact that in most hierarchical domains, it is more important to choose among subgoals than behaving optimally while pursuing a subgoal. Moreover, macro-actions can be gradually refined by replacing them with the instance that has a higher cumulative reward each time the corresponding subgoal is rediscovered.

Altogether, drawing on the already known benefits of MCTS, our approach aims to achieve the following properties:

**Generality** Only a subgoal predicate is needed in addition to the generative model of the domain. Unlike other methods, no information is needed about how to achieve a subgoal, from where it is reachable, how valuable it is, and how many macro-actions are available from each state.


**Scalability** The size of the state and action spaces of the domain is irrelevant to the size of the internal lookahead tree. Its branching factor merely depends on the actual number of reachable subgoals in each state. Its depth depends on the specified horizon and the length of each macro-action.


**Robustness** The performance of the algorithm is insensitive to a loose definition of subgoals. This enables the use of heuristics that can be generated with little domain knowledge, possibly using a subgoal discovery method.

**Efficiency** Sampling macro-actions can be implemented efficiently and incurs little overhead to the base MCTS algorithm. Further speedup can be achieved by running multiple instances of the algorithm in parallel, each with a low action coverage.
