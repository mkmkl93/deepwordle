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

vector< vector<int> > BaseSolver::check_determinizm(const vector<string> &checked_guesses,
													vector< vector<int> > &counter,
													vector< vector<int> > &sum,
													vector< vector<int> > &maxi) {
	vector<size_t> checked_guesses_ids;
	checked_guesses_ids.reserve(checked_guesses.size());

	for (const string &s : checked_guesses)
		checked_guesses_ids.emplace_back(lower_bound(guesses.begin(), guesses.end(), s) - guesses.begin());

	counter = vector< vector<int> >(wordSize + 1, vector<int>(wordSize + 1));
	sum = vector< vector<int> >(wordSize + 1, vector<int>(wordSize + 1));
	maxi = vector< vector<int> >(wordSize + 1, vector<int>(wordSize + 1));

	for (size_t sol_id = 0; sol_id < solution_ids.size(); sol_id++) {
		string s = solutions[sol_id];

		vector< string > answers(wordSize, string(wordSize, '-'));
		cout << s << "\n";
		DeadLetters dl(wordSize);
		for (size_t guess_id : checked_guesses_ids) {
			dl.addResponse(guesses[guess_id], solutionClue[guess_id][sol_id]);
			cout << guesses[guess_id] << " " << solutionClue[guess_id][sol_id].code << "\n";

			for (int i = 0; i < wordSize; i++) {
				for (int j = 0; j < wordSize; j++) {
					if (s[i] == guesses[guess_id][j]) {
						answers[i][j] = max(answers[i][j], solutionClue[guess_id][sol_id].code[j]);
					}
				}
			}
		}

		for (int i = 0; i < wordSize; i++) {
			cout << s[i] << " " << answers[i] << "\n";
		}

		int green = dl.green_letters();
		int yellow = dl.yellow_letters();
		cout << "green: " << green << " yellow: " << yellow << "\n";

		int coherent = dl.coherent_guesses();
		cout << "coherent: " << coherent << "\n";

		assert(0 <= green && green <= 5);
		assert(0 <= yellow && yellow <= 5);
		assert(yellow + green <= 5);
		counter[green][yellow]++;
		sum[green][yellow] += coherent;
		maxi[green][yellow] = max(maxi[green][yellow], coherent);
	}

	return counter;
}
