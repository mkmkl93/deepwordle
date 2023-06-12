#include "static_solver.h"

void StaticSolver::showPartition(vector<int> &known_guesses) {
	unordered_map<long long, bool> counter;
	int maxi_counter = 0;

	size_t n = guesses.size();

	for (int guess = 0; guess < n; guess++) {
		long long v = 0;

		for (const int &id : known_guesses) {
			v <<= 9;
			v |= (int)solutionClue[id][guess];
		}

		if (counter[v]) {
			maxi_counter++;

			if (maxi_counter >= 6)
				return;
		}

		counter[v] = true;
	}

	cout << elapsed_time() << " " ;

	for (int i : known_guesses)
		cout << guesses[i] << " ";

	cout << maxi_counter << " ";
	cout << "\n";

	cout.flush();
}

void StaticSolver::showPartition(vector<string> &&known_guesses) {
	vector<int> guesses_id;
	set<char> chars;

	for (const string &s : known_guesses) {
		cout << s << " ";

		for (const char c : s)
			chars.insert(c);
	}

	cout << "unique letters: " << chars.size() << " absent: ";
	for (char c = 'a'; c <= 'z'; c++) {
		if (chars.find(c) == chars.end()) {
			cout << c << " ";
		}
	}
	cout << "\n";

	for (const string &s : known_guesses) {
		size_t place = lower_bound(guesses.begin(), guesses.end(), s) - guesses.begin();

		if (guesses[place] != s)
			throw runtime_error("Guess not allowed");

		guesses_id.emplace_back(place);
	}

	showPartition(guesses_id);
}


void StaticSolver::solve() {
	start_time = chrono::system_clock::now();

	const int max_depth = 6;
	vector<int> thresholds = {0, 4, 9, 14, 19, 24};

	vector< vector<int> > space(max_depth);
	for (int i = 0; i < max_depth; i++) {
		space[i] = solution_ids;
	}
	assert(thresholds.size() == space.size());

	solve(max_depth, space, thresholds);
}

void StaticSolver::solve(const int &max_depth, const vector<vector<int>> &space, const vector<int> &thresholds) {
	vector< bitset<ALPHABET_SIZE> > solution_chars;
	for (const string &s : guesses) {
		bitset<ALPHABET_SIZE> bits;

		for (const char &c : s)
			bits[c - 'a'] = true;

		solution_chars.emplace_back(bits);
	}

	vector<bool> repeat = { false };
	for (size_t i = 1; i < space.size(); i++)
		repeat.emplace_back(space[i] == space[i - 1]);

	int nThread = clamp(int(thread::hardware_concurrency()), 1, 10);

	ctpl::thread_pool pool(nThread);

	for (int i = 0; i < space[0].size(); i++) {
		pool.push([this, i, &max_depth, &space, &thresholds, &solution_chars, &repeat](int id) {

			vector<int> current_res(space.size());
			current_res[0] = space[0][i];

			bitset<ALPHABET_SIZE> used_chars = solution_chars[ space[0][i] ];

			explore(2, max_depth, current_res, used_chars, thresholds, space, solution_chars, repeat, i + 1);

			cout << elapsed_time() << " Job " << i << " has ended by thread " << id << "\n";
			cout.flush();
		});
	}
}


void StaticSolver::explore(const int depth,
						   const int &max_depth,
						   vector<int> &current_res,
						   const bitset<ALPHABET_SIZE> &used_chars_base,
						   const vector<int> &thresholds,
						   const vector< vector<int> > &space,
						   const vector< bitset<ALPHABET_SIZE> > &solution_chars,
						   const vector<bool> &repeat,
						   const size_t start_i) {

	const vector<int> &v = space[depth];
	size_t n = v.size();
	size_t begin = repeat[depth] ? start_i : 0;

	for (size_t i = begin; i < n; i++) {
		bitset<ALPHABET_SIZE> used_chars = used_chars_base | solution_chars[ v[i] ];

		if (used_chars.count() < thresholds[depth])
			continue;

		current_res[depth] = v[i];

		if (depth + 1 == space.size()) {
			showPartition(current_res);
		} else {
			explore(depth + 1, max_depth, current_res, used_chars, thresholds, space, solution_chars, repeat, i + 1);
		}
	}
}

void StaticSolver::findSolutionInOne(const vector<vector<string>> &arr) {
	for (const vector<string> &sol : arr) {
		vector<int> sol_ids;

		for (const string &s : sol) {
			size_t place = lower_bound(guesses.begin(), guesses.end(), s) - guesses.begin();

			if (guesses[place] != s)
				throw runtime_error("Guess not allowed");

			sol_ids.emplace_back(place);
		}

		for (int i = 0; i < guesses.size(); i++) {
			sol_ids.emplace_back(i);

			unordered_map<long long, bool> counter;
			int maxi_counter = 0;

			size_t n = guesses.size();

			for (int guess = 0; guess < n; guess++) {
				long long v = 0;

				for (const int &id : sol_ids) {
					v <<= 9;
					v |= (int)solutionClue[id][guess];
				}

				if (counter[v]) {
					maxi_counter++;

					break;
				}

				counter[v] = true;
			}

			if (maxi_counter == 0) {
				cout << "Solution found: ";
				for (const int &id : sol_ids) {
					cout << guesses[id] << " " ;
				}

				vector< vector<int> > count, sum, maxi;
				vector<string> sol_tmp = sol;
				sol_tmp.emplace_back(guesses[i]);

				check_determinizm(sol_tmp, count, sum, maxi);

				int tmp = 0;
				for (const auto &line : sum)
					for (const auto &v : line)
						tmp += v;

				cout << "\tsum: " << tmp << "\n";


			}

			sol_ids.pop_back();
		}
	}

}
