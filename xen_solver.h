#ifndef WORDLE_XEN_SOLVER_H
#define WORDLE_XEN_SOLVER_H

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include <queue>
#include <deque>
#include <array>
#include <map>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <cassert>
#include <fstream>
#include <future>
#include <thread>
#include <chrono>
#include <iomanip>
#include <thread>
#include <atomic>
#include <numeric>
#include <filesystem>
#include "words_handler.h"

using int64 = int64_t;
using namespace std;

class XenSolver {
public:
	void solve(vector<string> &guess, vector<string> &solutions);

private:
	struct Path {
		int guess = -1;
		int nSolution = -1;
		int maxDepth = 0;	// worst case number of guesses, including this guess
		double ev = 0;	// expected number of guess, including this guess
		int64 solutionsPack = -1;	// up to 5 possible solutions packed 12-bits each into 64-bit int
		map<int, Path> choices;   // [243]
	};

	double timeDiff(const std::chrono::time_point<std::chrono::system_clock> &t0, const std::chrono::time_point<std::chrono::system_clock> &t1);

	double since(const std::chrono::time_point<std::chrono::system_clock> &t0);

	static int getResponse(string guess, string solution);

	static vector<int> splitIntoPartsCount(vector<short> const &solutions, vector<short> const & solutionClue);

	static vector<vector<short>> splitIntoParts(vector<short> const &solutions, vector<short> const & solutionClue);

	static int splitIntoPartsMax(vector<short> const &solutions, vector<short> const & solutionClue);

	static string responseToString(int r);

	static array<int, 5> collectDead(string w, int response, array<int, 5> const & deadLetters);

	static Path exploreGuess(vector<short> const & solution, int g, vector<vector<short>> const & solutionClue, vector<string> const & guess,
							 array<int, 5> const & deadLetters, int depthToBeat, int effort, bool shallow, int callDepth);

	static void showPath(ofstream & treefile, string const & prefix, Path const & path, vector<string> const & guess, vector<vector<short>> const & solutionClue, int depth = 0);

	static void pushSolutions(vector<int> & sols, Path const & path);

	static void showSolutions(ostream & tableFile, Path const & path, vector<string> const & guess);

	static void showTable(ostream & tableFile, Path const & path, vector<string> const & guess);
};


#endif //WORDLE_XEN_SOLVER_H