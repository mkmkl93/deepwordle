#include "static_solver.h"

void StaticSolver::showPartition(vector<int> &known_guesses) {
	unordered_map<long long, bool> counter;
	int maxi_counter = 0;

	size_t n = guesses.size();

	for (int guess = 0; guess < n; guess++) {
		long long v = 0;

		for (int &id : known_guesses) {
			v <<= 9;
			v |= solutionClue[id][guess];
		}

		if (counter[v])
			maxi_counter++;

		counter[v] = true;

		if (maxi_counter >= 6)
			return;
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

	for (auto s : known_guesses) {
		cout << s << " ";

		for (auto c : s)
			chars.insert(c);
	}

	cout << "letters: " << chars.size() << " absent: ";
	for (char c = 'a'; c <= 'z'; c++) {
		if (chars.find(c) == chars.end()) {
			cout << c << " ";
		}
	}
	cout << "\n";

	for (const string &s : known_guesses) {
		size_t place = lower_bound(guesses.begin(), guesses.end(), s) - guesses.begin();

		if (guesses[place] != s)
			throw "Guess not allowed";

		guesses_id.emplace_back(place);
	}

	showPartition(guesses_id);
}


void StaticSolver::solve() {
	start_time = chrono::system_clock::now();

	vector< bitset<26> > solution_chars;
	for (string &s : guesses) {
		bitset<26> bits;

		for (char c : s)
			bits[c - 'a'] = true;

		solution_chars.emplace_back(bits);
	}

	vector< vector<int> > space;
	space.emplace_back(solution_ids);
	space.emplace_back(solution_ids);
	space.emplace_back(solution_ids);
	space.emplace_back(solution_ids);
	space.emplace_back(solution_ids);
	space.emplace_back(solution_ids);
	vector<int> treshholds = {0, 4, 9, 14, 19, 24};
	assert(treshholds.size() == space.size());

	vector<bool> repeat = { false };
	for (size_t i = 1; i < space.size(); i++)
		repeat.emplace_back(space[i] == space[i - 1]);

	int nThread = clamp(int(thread::hardware_concurrency()), 1, 10);

	ctpl::thread_pool pool(nThread);

	for (int i = 0; i < space[0].size(); i++) {
		pool.push([this, i, &space, &treshholds, &solution_chars, &repeat](int id) {
			vector<int> current_res = { space[0][i] };
			current_res.resize(space.size());

			bitset<26> used_chars = solution_chars[ space[0][i] ];

			explore(1, current_res, used_chars, treshholds, space, solution_chars, repeat, i + 1);

			cout << elapsed_time() << " Job " << i << " has ended by thread " << id << "\n";
			cout.flush();
		});
	}
}

void StaticSolver::explore(int depth, vector<int> &current_res, bitset<26> &used_chars_base, vector<int> &treshholds,
						   vector< vector<int> > &space,
						   vector< bitset<26> > &solution_chars,
						   vector<bool> &repeat,
						   size_t start_i) {

	vector<int> &v = space[depth];
	size_t n = v.size();
	size_t begin = repeat[depth] ? start_i : 0;

	for (size_t i = begin; i < n; i++) {
		bitset<26> used_chars = used_chars_base | solution_chars[ v[i] ];

		if (used_chars.count() < treshholds[depth]) {
			continue;
		}

		current_res[depth] = v[i];

		if (depth + 1 == space.size()) {
			showPartition(current_res);
		} else {
			explore(depth + 1, current_res, used_chars, treshholds, space, solution_chars, repeat, i + 1);
		}
	}
}
