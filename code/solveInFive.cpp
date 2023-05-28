#include <string>
#include <vector>
#include "words_handler.h"
#include "deep_solver.h"
#include "xen_solver.h"
#include "greedy_solver.h"
#include "static_solver.h"
#include "utils.h"
#include "semi_static_solver.h"

void show_static_solution_metrics(const vector<string> &sol) {
	vector<string> solutions = WordsHandler::solutions();
	vector<string> guess = solutions;

	StaticSolver staticSolver = StaticSolver(guess, solutions);

	vector< vector<int> > counter, sum, maxi;
	staticSolver.check_determinizm(sol, counter, sum, maxi);

	vector< vector<double> > avg(sum.size(), vector<double>(sum[0].size()));
	for (size_t i = 0; i < sum.size(); i++) {
		for (size_t j = 0; j < sum[i].size(); j++) {
			if (counter[i][j])
				avg[i][j] = 1.0 * sum[i][j] / counter[i][j];
		}
	}

	cout << "Solution: ";
	for (const string &s : sol)
		cout << s << " ";
	cout << "\n";

	cout << "Counter: \n";
	print_python_array(counter);
	cout << "Avg: \n";
	print_python_array(avg);
	cout << "Maxi: \n";
	print_python_array(maxi);

	int tmp = 0;
	for (const auto &i : sum)
		for (const auto &j : i)
			tmp += j;

	cout << "sum: " << tmp << "\n";
}

void compare_static_solutions() {
	vector<string> sol = {"gawky", "spend", "jumbo", "fritz", "chill", "verge", "robot"};

	show_static_solution_metrics(sol);
}


int main() {
	vector<string> solutions = WordsHandler::solutions();
//	solutions.resize(10);
	//vector<string> guess = readWordFile("guesses5.txt");
	vector<string> guess = WordsHandler::guesses();

	sort(solutions.begin(), solutions.end());
	guess = solutions;


//	XenSolver xen_solver = XenSolver();
//	xen_solver.solve(guess, solutions);

//	DeepSolver deepSolver = DeepSolver();
//	deepSolver.solve(guess, solutions);

//	GreedySolver greedySolver = GreedySolver();
//	greedySolver.solve(guess, solutions);

	StaticSolver staticSolver = StaticSolver(guess, solutions);
	staticSolver.solve();

//	compare_static_solutions();

	SemiStaticSolver semiStaticSolver = SemiStaticSolver(guess, solutions);
//	semiStaticSolver.solve();
//	semiStaticSolver.printDynamicArray({"bugle", "champ", "downy", "first"});
//	semiStaticSolver.showMaxPartition({"bugle", "champ", "downy", "first"});
//	cout << Response("00011") << " " << Response("00001") << " " << Response("02000");

	return 0;
}
