#include "deep_solver.h"

void DeepSolver::solve(vector<string> &solutions, vector<string> guess) {
	wordSize = solutions[0].size();
	answerSize = 1;
	for (int i = 0; i < wordSize; i++)
		answerSize *= 3;

	vector<int> solution_ids = getSolutionsId(solutions, guess);

	int nGuess = int(guess.size());
	int nSolution = int(solution_ids.size());

	vector< vector< vector< int > > > partitions(nGuess);   // [nguess][243][nsol]
	vector< vector< Response > > solutionClue(nGuess);	// [nguess][nguess] -> clue

//		int nThread = clamp(int(thread::hardware_concurrency()), 2, 32);
	int nThread = clamp(int(thread::hardware_concurrency()), 1, 1); //TODO zwiększyć

	solve_clues(guess, solution_ids, nGuess, nSolution, partitions, solutionClue, nThread);

	vector<int> index(nGuess);
	iota(index.begin(), index.end(), 0);

	vector<int> favs = index;
	vector<Path> favResults(favs.size());
	explore(guess, solution_ids, solutionClue, nThread, favs, favResults);

	for (int ifav = 0; ifav < int(favs.size()); ++ifav) {
		cout << guess[favs[ifav]] << " " << right << setw(6) << favResults[ifav].maxDepth << endl;
		showPaths(favResults[ifav], guess);
	}

	write_worst_paths_to_file(guess, favResults);


	cout << endl;
}

void DeepSolver::solve_clues(const vector<string> &guess, const vector<int> &solution_ids, int nGuess, int nSolution,
				 vector< vector< vector< int > > > &partitions, vector< vector< Response > > &solutionClue,
				 const int &nThread) {
	auto t0 = std::chrono::system_clock::now();
	vector<thread> threads;

	for (int it = 0; it < nThread; ++it) {
		threads.emplace_back([this, it, nGuess, nThread, nSolution, &guess, &solution_ids, &partitions, &solutionClue]() {
			for (int ig = it * nGuess / nThread; ig < (it + 1) * nGuess / nThread; ++ig) {
				partitions[ig].resize(answerSize);
				solutionClue[ig].resize(nGuess);

				for (int is = 0; is < nSolution; ++is) {
					Response response = getResponse(guess[ig], guess[solution_ids[is]]);
					partitions[ig][response].push_back(solution_ids[is]);
					solutionClue[ig][solution_ids[is]] = response;
				}
			}
		} );
	}
	for (auto &t : threads)
		t.join();
	cout << fixed << setprecision(3) << "solve_clues time " << " " << since(t0) << endl;
}

Path DeepSolver::exploreGuess(vector<int> &solution, int g, vector<vector<Response>> &solutionClue, vector<string> &guess,
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

			best.maxDepth = UNSOLVABLE;

			for (int i = 0; i < nGuess; ++i) {
				Path p = exploreGuess(sols, i, solutionClue, guess, dl2);

				if (p.maxDepth < best.maxDepth)
					best = p;
			}
		}

		result.nextPath[ip] = best;

		if (best.maxDepth > result.maxDepth || result.maxDepth == UNSOLVABLE)
			result.maxDepth = best.maxDepth;
	}

	result.guess = g;
	result.maxDepth++;

	return result;
}

void DeepSolver::write_worst_paths_to_file(vector<string> &guess, vector<Path> &favResults) {
	ofstream file;

	file.open(filesystem::path(string("../results/summary_ev.csv")));
	file << "guess,max guesses\n";
	for (int i = 0; i < (int)favResults.size(); i++) {
		file << guess[i] << "," << favResults[i].maxDepth << "\n";
	}

	file.close();
}

void DeepSolver::explore(vector<string> &guess, vector<int> &solution_ids, vector<vector<Response>> &solutionClue,
			 int nThread, vector<int> &favs, vector<Path> &favResults)  {
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

				DeadLetters dl(wordSize);
				Path path = exploreGuess(solution_ids, favs[ifav], solutionClue, guess, dl);

				favResults[ifav] = path;
				nDone++;

				double dt = since(te0);
				double done = double(nDone) / nFav;
				double rem = dt / done - dt;

				cout << setw(5) << it << " " << " " << setw(4) << ifav << "  " << guess[favs[ifav]] << "  "
					 << fixed << right << setw(6) << favResults[ifav].maxDepth << " "
					 << setw(7) << setprecision(2) << since(tf0) / 60 << " min     "
					 << setw(5) << setprecision(1) << done * 100 << "% done  "
					 << setw(5) << setprecision(0) << rem / 60 << " min to go" << endl;

				// *** "results" directory needs to already exist ***

				// the entire decision tree for this starting word
//					ofstream treefile;
//					treefile.open(std::filesystem::path(string("results/tree ") + guess[favs[ifav]] + ".txt"));
//					showPaths(treefile, "", path, guess, solutionClue);
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

vector<int> DeepSolver::splitIntoPartsCount(vector<int> &solutions, vector<Response> &solutionClue) {
	vector<int> count(answerSize);

	for (int sol : solutions)
		count[solutionClue[sol]] += 1;

	return count;
}

vector<vector<int>> DeepSolver::splitIntoParts(vector<int> &solutions, vector<Response> &solutionClue) {
	vector<int> count = splitIntoPartsCount(solutions, solutionClue);
	vector<vector<int>> parts(answerSize);

	for (int i = 0; i < answerSize; ++i)
		parts[i].reserve(count[i]);

	for (int sol : solutions)
		parts[solutionClue[sol]].push_back(sol);

	return parts;
}

int DeepSolver::splitIntoPartsMax(vector<int> &solutions, vector<Response> &solutionClue) {
	vector<int> count = splitIntoPartsCount(solutions, solutionClue);

	return *max_element(count.begin(), count.end());
}

void DeepSolver::showPaths(Path path, vector<string> &guess) {
	ofstream file;

	file.open(filesystem::path("../results/tree/tree_" + guess[path.guess]));

	showP(file, "", path, guess);

	file.close();
}

void DeepSolver::showP(ofstream &f, string prefix, Path path, vector<string> &guess) {
	for (int i = 0; i < answerSize; i++) {
		Path &p = path.nextPath[i];

		if (i == answerSize - 1 && p.nsol >= 1) {
			string pre = prefix + guess[path.guess] + " " + Response(wordSize, i).pretty_string() + " " + to_string(p.nsol);
			f << pre << "\n";
		}

		if (p.nsol >= 1 && i != answerSize - 1) {
			string pre = prefix + guess[path.guess] + " " + Response(wordSize, i).pretty_string() + " " + to_string(p.nsol) + " -> ";
			showP(f, pre, p, guess);
		}
	}
}
