#include "greedy_solver.h"

Path GreedySolver::exploreGuess(vector<int> &solution, int g, vector<vector<Response>> &solutionClue, vector<string> &guess,
							  DeadLetters &dl) {
	Path result(answerSize);
	result.nsol = solution.size();

	int nGuess = int(guess.size());

	// Jakie hasła by dostały konkretną odpowiedź gdybyśmy zapytali o słowo o indeksie g
	vector<vector<int>> parts = splitIntoParts(solution, solutionClue[g]);

	for (int ip = 0; ip < answerSize; ++ip) {
		const vector<int> &sols = parts[ip];
		// Dane zapytanie g w ogóle niczego nie zmienia bo po podziale dostajemy dokładnie taki samy zbiór możliwych haseł
		if (solution.size() == sols.size()) {
			result.maxDepth = UNSOLVABLE;
			return result;
		}
	}

	// Sprawdzamy każdą możliwą odpowiedź wyroczni
	for (int ip = 0; ip < answerSize; ++ip) {
		// Lista haseł pasujące do danej odpowiedzi
		vector<int> &sols = parts[ip];
		if (sols.empty())
			continue;

		// Liczba pasujących haseł, jeżeli dostaniemy odpowiedź `ip`
		int nipSol = int(sols.size());
		Path best(answerSize);
		best.nsol = nipSol;
		// Jest tylko jedna możliwa odpowiedź ale nie jest to słowo `g`, bo nie dostaliśmy odpowiedzi 242
		// TODO szybkie rozwiązanie dla nipSol == 2

		if (nipSol == 1) {
			if (ip != answerSize - 1) {
				best.maxDepth = 1;
				best.guess = sols[0];

				Path next(answerSize);
				next.nsol = 1;
				next.guess = sols[0];
				next.maxDepth = 0;

				best.nextPath[answerSize - 1] = next;
			} else {
				best.nsol = 1;
				best.maxDepth = 0;
				best.guess = sols[0];
			}
		} else {
			// TODO posortować po maksymalnym poddrzewie zapytania
			// Nowy stan martwych liter, jeżeli zgadujemy hasło `guess[g]`, dostaliśmy odpowiedź `ip` i początkowo mieliśmy stan `deadLetters`
			DeadLetters dl2 = dl;
			dl2.addResponse(guess[g], Response(wordSize, ip));

			vector<pair<int, int>> maxPart;
			for (int ig = 0; ig < nGuess; ++ig) {
				maxPart.emplace_back(splitIntoPartsMax(sols, solutionClue[ig]), ig);
			}

			sort(maxPart.begin(), maxPart.end());

			best = exploreGuess(sols, maxPart[0].second, solutionClue, guess, dl2);
		}

		result.nextPath[ip] = best;

		if (best.maxDepth > result.maxDepth || result.maxDepth == UNSOLVABLE)
			result.maxDepth = best.maxDepth;
	}

	result.guess = g;
	result.maxDepth++;

	return result;
}