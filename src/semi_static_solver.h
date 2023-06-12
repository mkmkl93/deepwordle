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

	void solve(int max_depth,
			   const vector< int > &thresholds);

	void explore(int depth,
				 const int &max_depth,
				 vector<int> &current_res,
				 bitset<ALPHABET_SIZE> used_chars_base,
				 const vector<int> &thresholds,
				 const vector< bitset<ALPHABET_SIZE> > &solution_chars,
				 int start_i);

	void checkPartition(const vector<int> &known_guesses);

	void showMaxPartition(const vector<int> &known_guesses);

	void showMaxPartition(const vector<string> &&known_guesses);

	void printDynamicArray(const vector<string> &&known_guesses);

	void printDynamicArray(const vector<int> &known_guesses);
};

#endif //WORDLE_SEMI_STATIC_SOLVER_H
