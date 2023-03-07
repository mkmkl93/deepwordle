//  Created by Xan Gregg on 1/12/22.
//

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
using ans_t = short;

std::chrono::time_point<std::chrono::system_clock> now() {
	return std::chrono::system_clock::now();
}

double timeDiff(const std::chrono::time_point<std::chrono::system_clock> &t0, const std::chrono::time_point<std::chrono::system_clock> &t1) {
	return std::chrono::duration<double>(t1 - t0).count();
}

double since(const std::chrono::time_point<std::chrono::system_clock> &t0) {
	return timeDiff(t0, std::chrono::system_clock::now());
}

// given a guess and the solution, return the response clue packed into a base-3 integer.
// the low-order digit is the last character.
// 0 == no match, 1 == right letter, wrong place, 2 == right letter in right place
// Using Wordle rules for duplicate letters, which are not treated the same
static ans_t getResponse(string guess, string solution) {
	ans_t g = 0;
	for (int i = 0; i < 5; ++i) {
		if (i > 0)
			g *= 3;
		if (guess[i] == solution[i]) {
			g += 2;
			guess[i] = ' ';
			solution[i] = ' ';
		}
	}
	ans_t y = 0;
	for (int i = 0; i < 5; ++i) {
		if (i > 0)
			y *= 3;
		if (guess[i] == ' ')
			continue;
		auto loc = solution.find(guess[i]);
		if (loc != std::string::npos) {
			y += 1;
			solution[loc] = ' ';
		}
	}
	return g + y;
}

// given a list of possible solutions and the solutionClue data for a specific guess word,
// return a separate list for each of the 243 possible responses.
static vector<int> splitIntoPartsCount(vector<ans_t> const &solutions, vector<ans_t> const & solutionClue) {
	vector<int> count(243);
	for (ans_t sol : solutions) {
		count[solutionClue[sol]] += 1;
	}
	return count;
}

// given a list of possible solutions and the solutionClue data for a specific guess word,
// return a separate list for each of the 243 possible responses.
static vector<vector<ans_t>> splitIntoParts(vector<ans_t> const &solutions, vector<ans_t> const & solutionClue) {
	vector<int> count = splitIntoPartsCount(solutions, solutionClue);
	vector<vector<ans_t>> parts(243);
	for (int i = 0; i < 243; ++i)
		parts[i].reserve(count[i]);
	for (ans_t sol : solutions) {
		parts[solutionClue[sol]].push_back(sol);
	}
	return parts;
}

// size of the largest partition
static int splitIntoPartsMax(vector<ans_t> const &solutions, vector<ans_t> const & solutionClue) {
	vector<int> count = splitIntoPartsCount(solutions, solutionClue);
	return *max_element(count.begin(), count.end());
}

// read a file of words into a vector
static vector<string> readWordFile(string const & fn) {
	vector<string> words;
	ifstream wordFile(fn);
	string w;
	while (getline(wordFile, w)) {
		words.push_back(w);
	}
	wordFile.close();
	return words;
}

// show the response in a printable form, using the Wordle Unicode characters
static string responseToString(int r) {
	vector<string> code{"⬜", "\xF0\x9F\x9F\xA8", "\xF0\x9F\x9F\xA9"};   // "\uD83D\uDFE8","\uD83D\uDFE9",
	string s;
	int m = 81;
	for (int i = 0; i < 5; i++) {
		s += code[(r / m) % 3];
		m /= 3;
	}
	return s;
}

// given a guess word, w, and its response, update data on 'dead' letters by position.
// dead letters can't possible appear at a given position (position 0 is the first letter of a word)
// Doesn't have to be perfect -- just used as a hint to reduce the list of words to try.
static array<int, 5> collectDead(string w, int response, array<int, 5> const & deadLetters) {
	array<int, 5> dead = deadLetters;
	array<int, 5> code;
	int m = 81;
	for (int i = 0; i < 5; i++) {
		code[i] = (response / m) % 3;
		m /= 3;
	}
	for (int i = 0; i < 5; i++) {
		if (code[i] == 0) {
			// letter does not appear; add to dead list
			bool hasMultiple = false;	// unless it occurs multiple times in w and some are non-zero
			for (int j = 0; j < 5; ++j) {
				if (i != j && code[j] != 0 && w[i] == w[j]) {
					hasMultiple = true;
					break;
				}
			}
			if (hasMultiple)
				dead[i] |= 1 << (w[i] - 'a');	// not here
			else {
				for (int j = 0; j < 5; ++j)
					dead[j] |= 1 << (w[i] - 'a');	// not anywhere
			}
		}
		else if (code[i] == 1) {
			dead[i] |= 1 << (w[i] - 'a');	// not here
		}
	}
	return dead;
}

// decision tree

struct Path {
	int maxDepth = 999;	// worst case number of guesses, including this guess
	vector<int> solution;
};

// Main work function to recursively explore and optimize the decisition tree from a given starting point.
/* solution - vector z id rozwiązań z tablicy zgadywania "guess"
 * g - którym rozwiązaniem się aktualnie zajmujemy
 * solutionClue[i][j] - jaką odpowiedź dostaniemy jeżeli zapytamy o 'i' a hasłem jest 'j'
 * guess - lista wyrazów do zgadywania
 * deadLetters - które litery na jakiej pozycji są dozwolone
 * depthToBeat - ???
 */

map< tuple<int, array<int, 5>>, int> mapa;

static Path exploreGuess(vector<ans_t> const &solution, int g, vector<vector<ans_t>> const &solutionClue, vector<string> const &guess,
						 array<int, 5> const &deadLetters) {
//	cout << "exploreGuess g: " << g << " depthToBeat: " << depthToBeat << "\n";

	mapa[ {g, deadLetters} ]++;

	Path path;
	path.maxDepth = -1;
	int nGuess = int(guess.size()); // ~3000

	// Jakie hasła by dostały konkretną odpowiedź gdybyśmy zapytali o słowo o indeksie g
	const vector<vector<ans_t>> &parts = splitIntoParts(solution, solutionClue[g]);

	for (int ip = 0; ip < 242; ++ip) {
		const vector<ans_t> &sols = parts[ip];
		// Dane zapytanie g w ogóle niczego nie zmienia bo po podziale dostajemy dokładnie taki samy zbiór możliwych haseł
		if (solution.size() == sols.size()) {
			// must be a repeated or otherwise useless guess
			path.maxDepth = 999;
//			cout << "return 2\n";
			return path;
		}
	}

	// Sprawdzamy każdą możliwą odpowiedź wyroczni
	for (int ip = 0; ip < 242; ++ip) {
		// Lista haseł pasujące do danej odpowiedzi
		const vector<ans_t> &sols = parts[ip];
		if (sols.empty())
			continue;

		// Liczba pasujących haseł, jeżeli dostaniemy odpowiedź `ip`
		int nipSol = int(sols.size());
		// TODO ???
		Path best;
		// Jest tylko jedna możliwa odpowiedź ale nie jest to słowo `g`, bo nie dostaliśmy odpowiedzi 242
		if (nipSol == 1) {
			// only one solution, but it isn't guess[g] because the response is not 242
			best.maxDepth = 1;
			best.solution.emplace_back(sols[0]);
		} else if (nipSol == 2) {
			// Mamy dwie możliwość, więc no lepiej się nie da
			best.maxDepth = 2;
			best.solution.emplace_back(sols[1]);
			best.solution.emplace_back(sols[0]);
		} else {
			// Nowy stan martwych liter, jeżeli zgadujemy hasło `guess[g]`, dostaliśmy odpowiedź `ip` i początkowo mieliśmy stan `deadLetters`
			array<int, 5> deadLetters2 = collectDead(guess[g], ip, deadLetters);

			best.maxDepth = 999;

			for (int ig = 0; ig < nGuess; ++ig) {
				Path p = exploreGuess(sols, ig, solutionClue, guess, deadLetters2);
				if (p.maxDepth < best.maxDepth)
					best = p;
			}
		}

		if (best.maxDepth > path.maxDepth)
			path = best;
	}

//	cout << "return ok\n";
	path.solution.emplace_back(g);
	path.maxDepth += 1;
	return path;
}


static void showPath(ofstream &treefile, string const &prefix, Path const &path, vector<string> const &guess, vector<vector<ans_t>> const &solutionClue) {
//	for (int i = 0; i < path.maxDepth; i++) {
//		int g = path.solution.front();
//		int response = solutionClue[g][s];
//
//		treefile << prefix << guess[g];
//			treefile << " " << responseToString(response) << std::setw(4) << 1 << "  " << guess[s];
//		treefile << "\n";
//	}
//
//	if (path.choices.empty()) {
//		if (path.maxDepth <= 2 && path.nSolution <= 5 && path.solutionsPack >= 0) {
//			// expand the compressed path tree
//			int g = path.solutionsPack & 0x0fff;
//			for (int i = 0; i < min(5, path.nSolution); ++i) {
//				int s = (path.solutionsPack >> (i * 12)) & 0x0fff;
//				int response = solutionClue[g][s];
//				treefile << prefix << guess[g];
//				if (response != 242)
//					treefile << " " << responseToString(response) << std::setw(4) << 1 << "  " << guess[s];
//				treefile << endl;
//			}
//		}
//		else {
//			treefile << prefix << std::setw(6) << " " << path.maxDepth;
//			for (int i = 0; i < min(5, path.nSolution); ++i) {
//				treefile << " ** " << guess[(path.solutionsPack >> (i * 12)) & 0x0fff];
//			}
//			treefile << std::endl;
//		}
//	}
//	else {
//		for (auto & kv : path.choices) {
//			if (kv.second.nSolution > 0) {
//				std::stringstream ss;
//				ss << prefix << guess[path.guess] << " " << responseToString(kv.first) << std::setw(5) << kv.second.nSolution << "  ";
//				showPath(treefile, ss.str(), kv.second, guess, solutionClue, depth + 1);
//			}
//		}
//		if (path.solutionsPack >= 0) {	// indicates the guess is a solution
//			std::stringstream ss;
//			treefile << prefix << guess[path.guess] << endl;
//		}
//	}
}

static void pushSolutions(vector<int> & sols, Path const & path) {
//	if (path.choices.empty()) {
//		for (int i = 0; i < min(5, path.nSolution); ++i) {
//			sols.push_back((path.solutionsPack >> (i * 12)) & 0x0fff);
//		}
//	}
//	else {
//		if (path.solutionsPack >= 0)	// indicates the guess is a solution
//			sols.push_back(path.guess);
//		for (auto const & kv : path.choices)
//			pushSolutions(sols, kv.second);
//	}
}

static void showSolutions(ostream & tableFile, Path const & path, vector<string> const & guess) {
//	vector<int> sols;
//	pushSolutions(sols, path);
//	if (sols.size() != path.nSolution)
//		tableFile << sols.size() << ";";
//	for	(int i = 0; i < int(sols.size()); ++i) {
//		if (i > 0)
//			tableFile << ";";
//		tableFile << guess[sols[i]];
//	}
}

static void showTable(ostream & tableFile, Path const & path, vector<string> const & guess) {
	// "first guess,avg guesses,max guesses,first clue,n solutions,second guess,max guesses remaining,solutions"
//	for (int ip = 0; ip < 243; ++ip) {
//		if (path.choices.find(ip) == path.choices.end())
//			continue;	// impossible response -- no solutions
//		Path const & next = path.choices.at(ip);
//		if (next.guess < 0 || next.nSolution == 0) {
//			cout << "empty choice" << path.guess << " " << ip << endl;
//			continue;	// impossible response -- no solutions
//		}
//		tableFile << guess[path.guess] << "," << path.maxDepth << ","
//		<< responseToString(ip) << "," << next.nSolution << "," << guess[next.guess] << ","
//		<< (next.nSolution == 1 && next.solutionsPack == next.guess ? 0 : next.maxDepth - 1) << ",";
//		showSolutions(tableFile, next, guess);
//		tableFile << endl;
//	}
}

vector<ans_t> getSolutionsId(const vector<string> &solutions, const vector<string> &guess) {
	vector<ans_t> solution;
	for (auto const &s : solutions) {
		auto loc = find(guess.begin(), guess.end(), s);

		if (loc != guess.end())
			solution.push_back(ans_t(loc - guess.begin()));
		else
			cout << "*** solution not found " << s << endl;
	}

	return solution;
}

class WordleSolver {
	const int N;
	int ANSWER_SIZE; // 3^N

public:
	WordleSolver(int N) : N(N) {
		ANSWER_SIZE = 1;
		for (int i = 0; i < N; i++)
			ANSWER_SIZE *= 3;

		array<int, 5> arr;
	}

	void solve(vector<string> &solutions, vector<string> guess) {
		vector<ans_t> solution_ids = getSolutionsId(solutions, guess);

		int nGuess = int(guess.size()); // 3624
		int nSolution = int(solution_ids.size()); // 2315   // other words are allowed as guesses

		vector<vector<vector<ans_t>>> partitions(nGuess);   // [nguess][243][nsol]
		vector<vector<ans_t>> solutionClue(nGuess);	// [nguess][nguess] -> clue
//		int nThread = clamp(int(thread::hardware_concurrency()), 2, 32);
		int nThread = clamp(int(thread::hardware_concurrency()), 1, 1); //TODO zwiększyć

		solve_clues(guess, solution_ids, nGuess, nSolution, partitions, solutionClue, nThread);

		vector<int> index(nGuess);
		std::iota(index.begin(), index.end(), 0);


#if 0
		// use this branch to try out a few specific words
	//vector<string> favWords{"trend", "begin", "route", "stein", "first", "clout", "trunk", "clone", "raise", "trend", "intro", "inert", "tinge", "avoid", "adieu"};
	//vector<string> favWords{"shush", "yummy", "mamma", "mummy", "vivid", "puppy"};
	vector<string> favWords{"shush", "yummy", "mummy"};	// 6
	vector<int> favs;
	for (auto const & s : favWords) {
		favs.push_back(int(std::find(guess.begin(), guess.end(), s) - guess.begin()));
	}
#else
		vector<int> favs = index;
#endif
		vector<Path> favResults(favs.size());
		explore(guess, solution_ids, solutionClue, nThread, favs, favResults);

		for (int ifav = 0; ifav < int(favs.size()); ++ifav) {
			cout << guess[favs[ifav]] << " " << std::right << std::setw(6) << favResults[ifav].maxDepth << endl;
		}

		write_worst_paths_to_file(guess, favResults);

//		ofstream twoDeepFile;
//		twoDeepFile.open(std::filesystem::path(string("../results/summary_two-deep.csv")));
//		twoDeepFile << "first guess,avg guesses,max guesses,first clue,n solutions,second guess,max guesses remaining,solutions" << endl;
//		for (auto & s : wordstreams) {
//			twoDeepFile << s.str();
//		}
//		twoDeepFile.close();

		cout << endl;
	}

private:
	void write_worst_paths_to_file(vector<string> &guess, vector<Path> &favResults) {
		ofstream summFile;

		summFile.open(std::filesystem::path(string("../results/summary_ev.csv")));
		summFile << "guess,max guesses,solution\n";
		for (int i = 0; i < (int)favResults.size(); i++) {
			summFile << guess[i] << "," << favResults[i].maxDepth << ",";

			for (int j = (int)favResults[i].solution.size() - 1; j >= 0; j--) {
				summFile << guess[ favResults[i].solution[j] ];

				if (j != 0)
					summFile << " ";
			}

			summFile << "\n";
		}
		summFile.close();
	}

	void explore(const vector<string> &guess, const vector<ans_t> &solution_ids, const vector<vector<ans_t>> &solutionClue,
			int nThread, const vector<int> &favs, vector<Path> &favResults) const {
		vector<thread> ethreads;
		atomic<int> next = 0;
		atomic<int> nDone = 0;
		int nFav = int(favs.size());
		auto te0 = std::chrono::system_clock::now();

		cout << "  wątek  id  słowo      max\n";

		for (int it = 0; it < nThread; ++it) {
			ethreads.emplace_back([this, it, te0, &nFav, &nDone, &next, &favs, &guess, &solution_ids, &solutionClue, &favResults]() {
				for (int ifav = next++; ifav < nFav; ifav = next++) {
					auto tf0 = std::chrono::system_clock::now();

					favResults[ifav] = exploreGuess(solution_ids, favs[ifav], solutionClue, guess, array<int, 5>());
					nDone++;

					double dt = since(te0);
					double done = double(nDone) / nFav;	// these two atomics could be fetched out of sync but no big deal
					double rem = dt / done - dt;

					cout << setw(5) << it << " " << " " << setw(4) << ifav << "  " << guess[favs[ifav]] << "  "
						 << fixed << right << setw(6) << favResults[ifav].maxDepth << " "
						 << setw(7) << setprecision(2) << since(tf0)/60 << " min     "
						 << setw(5) << setprecision(1) << done * 100 << "% done  "
						 << setw(5) << setprecision(0) << rem/60 << " min to go" << endl;

					// *** "results" directory needs to already exist ***

					// the entire decision tree for this starting word
//					ofstream treefile;
//					treefile.open(std::filesystem::path(string("results/tree ") + guess[favs[ifav]] + ".txt"));
//					showPath(treefile, "", path, guess, solutionClue);
//					treefile.close();

//					showTable(wordstreams[ifav], path, guess);
//
//					summstreams[it] << guess[favs[ifav]] << "," << fixed << "," << path.maxDepth << "," << since(tf) << endl;
				}
			} );
		}
		for (auto & t : ethreads)
			t.join();
	}

	void solve_clues(const vector<string> &guess, const vector<ans_t> &solution_ids, int nGuess, int nSolution,
					 vector<vector<vector<ans_t>>> &partitions, vector<vector<ans_t>> &solutionClue,
					 const int &nThread) {
		auto t0 = std::chrono::system_clock::now();
		vector<thread> threads;

		for (int it = 0; it < nThread; ++it) {
			threads.emplace_back([this, it, nGuess, nThread, nSolution, &guess, &solution_ids, &partitions, &solutionClue]() {
				for (int ig = it * nGuess / nThread; ig < (it + 1) * nGuess / nThread; ++ig) {
					partitions[ig].resize(ANSWER_SIZE);
					solutionClue[ig].resize(nGuess);
					for (ans_t is = 0; is < nSolution; ++is) {
						ans_t response = getResponse(guess[ig], guess[solution_ids[is]]);
						partitions[ig][response].push_back(solution_ids[is]);
						solutionClue[ig][solution_ids[is]] = response;
					}
				}
			} );
		}
		for (auto & t : threads)
			t.join();
		cout << fixed << setprecision(3) << "solve_clues time " << " " << since(t0) << endl;
	}
};



int main() {
	vector<string> solutions = WordsHandler::solutions();
	solutions.resize(30);  // only the first 2315 are used as solutions
	//vector<string> guess = readWordFile("guesses5.txt");
	vector<string> guess = WordsHandler::guesses();

//	solutions = {"aaaaa", "baaaa", "caaaa", "daaaa", "eaaaa",// "faaaa", "gaaaa", "haaaa", "iaaaa", "jaaaa", "kaaaa"
//				"abbaa", "accaa", "acbaa", "abcaa", "addaa", "adcaa", "adbaa", "acdaa", "abdaa"
//	};
	sort(solutions.begin(), solutions.end());
	guess = solutions;
	auto solver = WordleSolver(5);

	solver.solve(solutions, guess);

	return 0;
}
