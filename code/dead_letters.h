//
// Created by michal on 12.03.23.
//

#ifndef WORDLE_DEAD_LETTERS_H
#define WORDLE_DEAD_LETTERS_H

#include <bitset>
#include <cassert>
#include "response.h"
#include <cstring>
#include <set>
#include <iostream>

using namespace std;

struct DeadLetters {
	vector< bitset<26> > dead;
	vector<char> known;
	int word_size;

	DeadLetters(size_t n) : word_size(n) { dead.resize(n); };

	void killOne(int i, char letter);

	void killAllButOne(int i, char letter);

	void addResponse(string w, Response r);

	void unionKnown(vector<char> v_known);

	bool matches(string s);

	bool operator<(const DeadLetters& dl) const;

	bool certain(int i);

	void tryToInfer();

	bool infer1();

	bool infer2();

	int green_letters();

	int yellow_letters();

	vector<char> certainLetters();

	int coherent_guesses(int layer = 0, vector<char> guess = {});

	vector<char> intersect(vector<char> a, vector<char> b);
};

#endif //WORDLE_DEAD_LETTERS_H
