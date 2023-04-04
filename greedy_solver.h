#ifndef WORDLE_GREEDY_SOLVER_H
#define WORDLE_GREEDY_SOLVER_H

#include "deep_solver.h"

class GreedySolver : public DeepSolver {
	Path exploreGuess(vector<int> &solution, int g, vector<vector<Response>> &solutionClue, vector<string> &guess,
					  DeadLetters &dl) override;
};

#endif //WORDLE_GREEDY_SOLVER_H
