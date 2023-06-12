#include "utils.h"

vector<int> getSolutionsId(vector<string> &solutions, vector<string> &guess) {
	vector<int> solution;

	for (const string &s : solutions) {
		auto loc = find(guess.begin(), guess.end(), s);

		if (loc != guess.end())
			solution.push_back(int(loc - guess.begin()));
		else
			throw runtime_error("solution not included in guess");
	}

	return solution;
}

Response getResponse(string guess, string solution) {
	assert(guess.size() == solution.size());

	int n = (int)guess.size();

	string res(n, '0');

	for (int i = 0; i < n; ++i) {
		if (guess[i] == solution[i]) {
			res[i] = '2';
			guess[i] = ' ';
			solution[i] = ' ';
		}
	}

	for (int i = 0; i < n; ++i) {
		if (guess[i] == ' ')
			continue;

		auto loc = solution.find(guess[i]);
		if (loc != string::npos) {
			res[i] = '1';
			solution[loc] = ' ';
		}
	}

	return Response(move(res));
}

double timeDiff(const chrono::time_point<chrono::system_clock> &t0, const chrono::time_point<chrono::system_clock> &t1) {
	return chrono::duration<double>(t1 - t0).count();
}

double since(const chrono::time_point<chrono::system_clock> &t0) {
	return timeDiff(t0, chrono::system_clock::now());
}

