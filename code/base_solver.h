#ifndef WORDLE_BASE_SOLVER_H
#define WORDLE_BASE_SOLVER_H

#include <chrono>
#include "response.h"
#include "utils.h"
using namespace std;

class BaseSolver {
public:
	BaseSolver() { }
	BaseSolver(vector<string> &guesses, vector<string> &solutions);

protected:
	chrono::time_point<chrono::system_clock> start_time;
	size_t wordSize;
	size_t answerSize; // 3^wordSize
	// [i][j] - What response we would get if we asked for i-th word and the response is j-th word
	vector< vector< Response > > solutionClue;
	vector<int> solution_ids;
	vector<string> guesses;
	vector<string> solutions;


	void solve_clues(const vector<string> &guess);

	string elapsed_time();
};

#endif //WORDLE_BASE_SOLVER_H
