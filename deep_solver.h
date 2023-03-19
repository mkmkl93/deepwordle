//
// Created by michal on 16.03.23.
//

#ifndef WORDLE_DEEP_SOLVER_H
#define WORDLE_DEEP_SOLVER_H

#include <thread>
#include <numeric>
#include <iomanip>
#include <filesystem>
#include <fstream>
#include "dead_letters.h"
#include "utils.h"

const int UNSOLVABLE = 9999;

struct Path {
	int guess;
	size_t nsol;

	int maxDepth = UNSOLVABLE;	// worst case number of guesses, including this guess
	vector<Path> nextPath;

	Path() = default;

	Path(size_t n) { nextPath.resize(n); }

	bool operator<=>(const Path&) const = default;
};

class DeepSolver {
public:
	void solve(vector<string> &solutions, vector<string> guess);

private:
	size_t wordSize;
	size_t answerSize; // 3^wordSize

	void solve_clues(const vector<string> &guess, const vector<int> &solution_ids, int nGuess, int nSolution,
					 vector<vector<vector<int>>> &partitions, vector<vector<Response>> &solutionClue,
					 const int &nThread);

	void explore(vector<string> &guess, vector<int> &solution_ids, vector<vector<Response>> &solutionClue,
			int nThread, vector<int> &favs, vector<Path> &favResults);

	/* Main work function to recursively explore and optimize the decision tree from a given starting point.
	 * solution - vector z id rozwiązań z tablicy zgadywania "guess"
	 * g - którym rozwiązaniem się aktualnie zajmujemy
	 * solutionClue[i][j] - jaką odpowiedź dostaniemy jeżeli zapytamy o 'i' a hasłem jest 'j'
	 * guess - lista wyrazów do zgadywania
	 * deadLetters - które litery na jakiej pozycji są dozwolone
	 * depthToBeat - głębokość do pokonania
	 */
	Path exploreGuess(vector<int> &solution, int g, vector<vector<Response>> &solutionClue, vector<string> &guess,
					  DeadLetters &dl);

	// given a list of possible solutions and the solutionClue data for a specific guess word,
	// return a separate list for each of the 243 possible responses.
	vector<int> splitIntoPartsCount(vector<int> &solutions, vector<Response> &solutionClue);

	// given a list of possible solutions and the solutionClue data for a specific guess word,
	// return a separate list for each of the 243 possible responses.
	vector<vector<int>> splitIntoParts(vector<int> &solutions, vector<Response> &solutionClue);

	// size of the largest partition
	int splitIntoPartsMax(vector<int> &solutions, vector<Response> &solutionClue);

	void write_worst_paths_to_file(vector<string> &guess, vector<Path> &favResults);

	void showPaths(Path path, vector<string> &guess);

	void showP(ofstream &f, string prefix, Path path, vector<string> &guess);

	string responseToString(int r);
};

#endif //WORDLE_DEEP_SOLVER_H
