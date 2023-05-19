#ifndef WORDLE_STATIC_SOLVER_H
#define WORDLE_STATIC_SOLVER_H

#include <vector>
#include <string>
#include <thread>
#include <unordered_map>
#include <iomanip>
#include <random>
#include <bitset>
#include <set>
#include "utils.h"
#include "base_solver.h"
#include "ctpl_stl.h"
using namespace std;


class StaticSolver : public BaseSolver {
	using BaseSolver::BaseSolver;
public:
	void solve();

	void explore(int depth,
				 vector<int> &current_res,
				 bitset<26> &used_chars_base,
				 vector<int> &treshholds,
				 vector< vector<int> > &space,
				 vector< bitset<26> > &solution_chars,
				 vector<bool> &repeat,
				 size_t start_i);

	void showPartition(vector<int> &known_guesses, bool show_partition = false);

	void showPartition(vector<string> &&known_guesses);

	void calc_difference();
};

#endif //WORDLE_STATIC_SOLVER_H
