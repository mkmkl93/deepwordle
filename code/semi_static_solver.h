#ifndef WORDLE_SEMI_STATIC_SOLVER_H
#define WORDLE_SEMI_STATIC_SOLVER_H


#include <vector>
#include <string>
#include <thread>
#include <map>
#include "utils.h"
#include "base_solver.h"
#include "ctpl_stl.h"
using namespace std;


class SemiStaticSolver : public BaseSolver {
	using BaseSolver::BaseSolver;
public:
	void solve();

	void explore(int depth,
				 int &max_depth,
				 vector<int> &current_res,
				 bitset<26> used_chars_base,
				 vector<int> thresholds,
				 vector< bitset<26> > &solution_chars,
				 size_t start_i);

	void checkPartition(const vector<int> &known_guesses);

	void showMaxPartition(const vector<int> &known_guesses);

	void showMaxPartition(const vector<string> &&known_guesses);

	void printDynamicArray(const vector<string> &&known_guesses);

	void printDynamicArray(const vector<int> &known_guesses);
};

#endif //WORDLE_SEMI_STATIC_SOLVER_H
