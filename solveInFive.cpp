#include <string>
#include <vector>
#include "words_handler.h"
#include "deep_solver.h"

int main() {
	vector<string> solutions = WordsHandler::solutions();
	solutions.resize(10);  // only the first 2315 are used as solutions
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