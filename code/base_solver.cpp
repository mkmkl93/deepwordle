#include "base_solver.h"

void BaseSolver::solve_clues(const vector<string> &guess) {

	size_t n = guess.size();

	solutionClue.resize(n);

	for (int i = 0; i < n; i++) {
		solutionClue[i].resize(n);

		for (int j = 0; j < n; j++) {
			Response response = getResponse(guess[i], guess[j]);
			solutionClue[i][j] = response;
		}
	}
}

string BaseSolver::elapsed_time() {
	int time_diff = (int)timeDiff(start_time, chrono::system_clock::now());

	if (!time_diff)
		time_diff++;

	int seconds = time_diff;
	int minutes = seconds / 60;
	int hours = minutes / 60;

	string res;

	if (hours)
		res += to_string(hours) + ":";

	if (minutes)
		res += to_string(minutes % 60) + ":";

	if (seconds)
		res += to_string(seconds % 60);

	return res;
}

BaseSolver::BaseSolver(vector<string> &guess, vector<string> &sols) {
	guesses = guess;
	solutions = sols;

	assert(guesses == solutions);

	wordSize = solutions[0].size();
	answerSize = 1;
	for (int i = 0; i < wordSize; i++)
		answerSize *= 3;

	solve_clues(solutions);
	solution_ids = getSolutionsId(solutions, guesses);
}
