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

	void solve(const int &max_depth,
			   const vector< vector<int> > &space,
			   const vector< int > &thresholds);

	void explore(int depth,
				 const int &max_depth,
				 vector<int> &current_res,
				 const bitset<26> &used_chars_base,
				 const vector<int> &thresholds,
				 const vector< vector<int> > &space,
				 const vector< bitset<26> > &solution_chars,
				 const vector<bool> &repeat,
				 size_t start_i);

	void showPartition(vector<int> &known_guesses);

	void showPartition(vector<string> &&known_guesses);

	void findSolutionInOne(const vector< vector< string > > &arr);
};

#endif //WORDLE_STATIC_SOLVER_H
