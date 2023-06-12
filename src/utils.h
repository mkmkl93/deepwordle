#ifndef WORDLE_UTILS_H
#define WORDLE_UTILS_H

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <chrono>
#include "response.h"
using namespace std;

// For every word in `solutions` get its id in `guess`
vector<int> getSolutionsId(vector<string> &solutions, vector<string> &guess);

// Given a guess and the solution, return the response
// 0 == no match, 1 == right letter, wrong place, 2 == right letter in right place
// Using Wordle rules for duplicate letters, which are not treated the same
Response getResponse(string guess, string solution);

double timeDiff(const chrono::time_point<chrono::system_clock> &t0, const chrono::time_point<chrono::system_clock> &t1);

double since(const chrono::time_point<std::chrono::system_clock> &t0);

template<typename T>
void print_python_array(const vector<vector<T>> &v) {
	cout << "[\n";
	for (size_t i = 0; i < v.size(); i++) {
		cout << "\t[";
		for (size_t j = 0; j < v[i].size(); j++) {
			cout << "" << v[i][j];

			if (j + 1 != v[i].size())
				cout << ", ";
		}
		cout << "]";

		if (i + 1 != v.size())
			cout << ", ";

		cout << "\n";
	}
	cout << "]\n";
}

constexpr int ALPHABET_SIZE = 26;

#endif //WORDLE_UTILS_H
