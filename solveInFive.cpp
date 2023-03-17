//  Created by Xan Gregg on 1/12/22.
//

#include <string>
#include <vector>
#include <queue>
#include <deque>
#include <map>
#include <set>
#include <chrono>
#include "words_handler.h"
#include "dead_letters.h"
#include "deep_solver.h"

using int64 = int64_t;
using namespace std;

std::chrono::time_point<std::chrono::system_clock> now() {
	return std::chrono::system_clock::now();
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

vector<int> filter(vector<string> const &guess, DeadLetters dl) {
	vector<int> res;

	for(int s = 0; s < (int)guess.size(); s++) {
		string str = guess[s];

		if(dl.matches(str))
			res.emplace_back(s);
	}

	return res;
}



static void showPath(ofstream &treefile, string const &prefix, Path const &path, vector<string> const &guess, vector<vector<int>> const &solutionClue) {
//	for (int i = 0; i < path.maxDepth; i++) {
//		int g = path.solution.front();
//		int response = solutionClue[g][s];
//
//		treefile << prefix << guess[g];
//			treefile << " " << responseToString(response) << std::setw(4) << 1 << "  " << guess[s];
//		treefile << "\wordSize";
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
	// "first guess,avg guesses,max guesses,first clue,wordSize solutions,second guess,max guesses remaining,solutions"
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

int main() {
	vector<string> solutions = WordsHandler::solutions();
	solutions.resize(20);  // only the first 2315 are used as solutions
	//vector<string> guess = readWordFile("guesses5.txt");
	vector<string> guess = WordsHandler::guesses();

//	solutions = {"aaaaa", "baaaa", "caaaa", "daaaa", "eaaaa", "faaaa", "gaaaa", "haaaa", "iaaaa", "jaaaa", "kaaaa"
				//"abbaa", "accaa", "acbaa", "abcaa", "addaa", "adcaa", "adbaa", "acdaa", "abdaa"
//	};
	sort(solutions.begin(), solutions.end());
	guess = solutions;
	DeepSolver deep_solver{};

	deep_solver.solve(solutions, guess);

	return 0;
}

// TODO zobaczyć jakie słowa są rozdzielane przez względnie mało pytań