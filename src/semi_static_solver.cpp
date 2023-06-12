#include "semi_static_solver.h"

void SemiStaticSolver::checkPartition(const vector<int> &known_guesses) {
	unordered_map<long long, int> counter;
	int maxi = 0;
	int non_trivial = 0;

	size_t n = guesses.size();

	for (int guess = 0; guess < n; guess++) {
		long long v = 0;

		for (const int &id : known_guesses) {
			v <<= 9;
			v |= int(solutionClue[id][guess]);
		}

		counter[v]++;

		maxi = max(maxi, counter[v]);

		if (counter[v] == 2)
			non_trivial++;

		if (maxi > 4 || non_trivial > 60)
			return;
	}

	for (int i : known_guesses)
		cout << guesses[i] << " ";
	cout << "\t";

	cout << "Maximal size: " << maxi << "\t";

	cout << "Non trivial: " << non_trivial << "\t";

	set<char> s;
	for (const int &kg : known_guesses)
		for (const char &c : guesses[kg])
			s.insert(c);
	cout << "Unique characters: " << s.size() << "\t";

	cout << elapsed_time() << "\n" ;

	cout.flush();
}

void SemiStaticSolver::showMaxPartition(const vector<int> &known_guesses) {
	unordered_map<long long, vector<int>> counter;
	vector<int> maxi;

	size_t n = guesses.size();
	int non_trivial = 0;

	for (int guess = 0; guess < n; guess++) {
		long long v = 0;

		for (const int &id : known_guesses) {
			v <<= 9;
			v |= (int)solutionClue[id][guess];
		}

		counter[v].emplace_back(guess);

		if (counter[v].size() > maxi.size())
			maxi = counter[v];

		if (counter[v].size() == 2)
			non_trivial++;
	}

	cout << elapsed_time() << " " ;

	cout << "Non trivial groups: " << non_trivial << "\n";

	for (int i : known_guesses)
		cout << guesses[i] << " ";

	maxi = counter[(4LL << 27) + (81 << 18) + (0 << 9) + 0];
	cout << "\nBiggest group: ";
	for (const int &i : maxi)
		cout << guesses[i] << " ";
	cout << "\n";

//	cout << "All groups:\n";
//	for (const auto &it : counter) {
//		if (it.second.size() > 1) {
//			for (const int i : it.second)
//				cout << guesses[i] << " ";
//			cout << "\n";
//		}
//	}

	cout.flush();
}


void SemiStaticSolver::showMaxPartition(const vector<string> &&known_guesses) {
	vector<int> guesses_id;

	for (const string &s : known_guesses) {
		size_t place = lower_bound(guesses.begin(), guesses.end(), s) - guesses.begin();

		if (guesses[place] != s)
			throw runtime_error("Guess not allowed");

		guesses_id.emplace_back(place);
	}

	showMaxPartition(guesses_id);
}


void SemiStaticSolver::solve() {
	start_time = chrono::system_clock::now();

	vector<int> thresholds = {4, 9, 14, 19};

	solve((int)thresholds.size(), thresholds);
}

void SemiStaticSolver::solve(const int max_depth,
							 const vector< int > &thresholds) {
	vector< bitset<ALPHABET_SIZE> > solution_chars;
	for (const string &s : solutions) {
		bitset<ALPHABET_SIZE> bits;

		for (const char &c : s)
			bits[c - 'a'] = true;

		solution_chars.emplace_back(bits);
	}

	const int nThread = clamp(int(thread::hardware_concurrency()), 1, 10);

	ctpl::thread_pool pool(nThread);

	for (int i = 0; i < solution_ids.size(); i++) {
		pool.push([this, i, &max_depth, thresholds, &solution_chars](int id) {
			vector<int> current_res(max_depth);
			current_res[0] = (int)i;
			bitset<ALPHABET_SIZE> used_chars = solution_chars[i];

			explore(1, max_depth, current_res, used_chars, thresholds, solution_chars, i + 1);

			cout << elapsed_time() << " Job " << i << " has ended by thread " << id << "\n";
			cout.flush();
		});
	}
}

void SemiStaticSolver::explore(const int depth,
							   const int &max_depth,
							   vector<int> &current_res,
							   const bitset<ALPHABET_SIZE> used_chars_base,
							   const vector<int> &thresholds,
							   const vector< bitset<ALPHABET_SIZE> > &solution_chars,
							   const int start_i) {
	for (int i = start_i; i < solution_ids.size(); i++) {
		bitset<ALPHABET_SIZE> used_chars = used_chars_base | solution_chars[i];

		if (used_chars.count() < thresholds[depth]) {
			continue;
		}

		current_res[depth] = i;

		if (depth + 1 == max_depth) {
			checkPartition(current_res);
		} else {
			explore(depth + 1, max_depth, current_res, used_chars, thresholds, solution_chars, i + 1);
		}
	}
}

void SemiStaticSolver::printDynamicArray(const vector<string> &&known_guesses) {
	vector<int> guesses_id;

	for (const string &s : known_guesses) {
		size_t place = lower_bound(guesses.begin(), guesses.end(), s) - guesses.begin();

		if (guesses[place] != s)
			throw runtime_error("Guess not allowed");

		guesses_id.emplace_back(place);
	}

	printDynamicArray(guesses_id);
}

void SemiStaticSolver::printDynamicArray(const vector<int> &known_guesses) {
	map<vector<int>, vector<int>> counter;
	vector<int> maxi;

	size_t n = guesses.size();

	for (int guess = 0; guess < n; guess++) {
		vector<int> k;
		k.reserve(known_guesses.size());

		for (const int &id : known_guesses) {
			k.emplace_back(solutionClue[id][guess]);
		}

		counter[k].emplace_back(guess);
	}

	int no = 1;
	cout << "All groups:\n";
	for (const auto &it : counter) {
		if (it.second.size() > 1) {
			cout << no++ << " & ";

			for (const int i : it.first)
				cout << Response(wordSize, i).latex_string() << " & ";

			for (const int i : it.second)
				cout << guesses[i] << " ";
			cout << "& ";

			if (it.second.size() == 2)
				cout << guesses[it.second[0]] << " ";
			cout << "\\\\ \n";
		}
	}

	cout.flush();

}
