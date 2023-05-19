#include "dead_letters.h"

void DeadLetters::killOne(int i, char letter) {
	assert('a' <= letter && letter <= 'z');
	dead[i][letter - 'a'] = true;
}

void DeadLetters::killAllButOne(int i, char letter) {
	assert(0 <= i && i <= 4);
	assert('a' <= letter && letter <= 'z');
	dead[i].set();
	dead[i][letter - 'a'] = false;
}

void DeadLetters::addResponse(string w, Response r) {
	string &code = r.code;

	vector<char> v_known;

	for (int i = 0; i < word_size; i++) {
		if (code[i] == '0') {
			bool hasMultiple = false, allOtherCorrect = true, allOtherFalse = true;
			for (int j = 0; j < word_size; ++j) {
				if (i != j && w[i] == w[j]) {
					hasMultiple = true;
					allOtherCorrect &= code[j] == '2';
					allOtherFalse &= code[j] == '0';
				}
			}
			if (hasMultiple) {
				if (allOtherCorrect) {
					for (int j = 0; j < word_size; j++) {
						if (w[j] != w[i] || code[j] == '0') {
							killOne(j, w[i]);
						}
					}
				} if (allOtherFalse) {
					for (int j = 0; j < word_size; ++j)
						killOne(j, w[i]);
				} else {
					killOne(i, w[i]);
				}
			} else {
				for (int j = 0; j < word_size; ++j)
					killOne(j, w[i]);
			}
		} else if (code[i] == '1') {
			v_known.emplace_back(w[i]);
			killOne(i, w[i]);
		} else {
			v_known.emplace_back(w[i]);
			killAllButOne(i, w[i]);
		}
	}

	unionKnown(v_known);

	vector<char> certain_letters = certainLetters();
	unionKnown(certain_letters);

	tryToInfer();
}

void DeadLetters::unionKnown(vector<char> v_known) {
	vector<char> res;
	sort(v_known.begin(), v_known.end());

	int i = 0, j = 0;
	while(i < known.size() && j < v_known.size()) {
		if (known[i] == v_known[j]) {
			res.emplace_back(known[i]);
			i++;
			j++;
		} else if (known[i] < v_known[j]) {
			res.emplace_back(known[i]);
			i++;
		} else {
			res.emplace_back(v_known[j]);
			j++;
		}
	}

	while(i < known.size()) {
		res.emplace_back(known[i]);
		i++;
	}

	while(j < v_known.size()) {
		res.push_back(v_known[j]);
		j++;
	}

	known = res;
}

bool DeadLetters::matches(string s) {
	assert(word_size == s.size());

	for (int i = 0; i < word_size; i++) {
		if (dead[i][s[i] - 'a'])
			return false;
	}

	sort(s.begin(), s.end());

	int j = 0;
	for (int i = 0; i < word_size; i++) {
		if (j < known.size() && s[i] == known[j])
			j++;
	}

	return j == known.size();
}

bool DeadLetters::operator<(const DeadLetters &dl) const {
	if (dead.size() != dl.dead.size())
		return dead.size() < dl.dead.size();

	for (int i = 0; i < (int)dead.size(); i++) {
		if (dead[i] != dl.dead[i])
			return dead[i].to_ulong() < dl.dead[i].to_ulong();
	}

	return known < dl.known;
}

int DeadLetters::green_letters() {
	int res = 0;

	for (int i = 0; i < word_size; i++) {
		if (certain(i))
			res++;
	}

	return res;
}

int DeadLetters::yellow_letters() {
//	return max((int)known.size() - green_letters(), 0);
	return known.size() - green_letters();
}

// Only works because there are 24 unique letters in the solution and not 25
void DeadLetters::tryToInfer() {
	bool changed = false;

	changed |= infer1();
	changed |= infer2();

	if (changed)
		tryToInfer();
}

bool DeadLetters::infer1() {
	vector<bool> sure(word_size);
	int sure_counter = 0;
	multiset<int> set;

	for (char c : known)
		set.insert(c - 'a');
	bool changed = false;

	for (int i = 0; i < word_size; i++) {
		if (certain(i)) {
			sure[i] = true;
			sure_counter++;

			for (int j = 0; j < dead[i].size(); j++) {
				if (!dead[i][j]) {
					auto it = set.find(j);
					assert(it != set.end());
					set.erase(it);
				}
			}
		}
	}

	int left_pos = word_size - sure_counter;
	for (int c : set) {
		int certain_death = 0;

		for (int i = 0; i < word_size; i++) {
			if (sure[i])
				continue;

			if (dead[i][c]) {
				certain_death++;
			}
		}

		int letter_counter = 0;
		for (int c2 : set) {
			if (c == c2)
				letter_counter++;
		}

		if (certain_death + letter_counter == left_pos) {
			changed = true;

			for (int i = 0; i < word_size; i++) {
				if (sure[i])
					continue;

				if (!dead[i][c]) {
					killAllButOne(i, 'a' + c);
				}
			}
		}
	}

	return changed;
}

bool DeadLetters::infer2() {
	if (known.size() != 5)
		return false;

	bool changed = false;
	vector< bool > sure(word_size);
	multiset<char> set;

	for (char c : known)
		set.insert(c);

	for (int i = 0; i < word_size; i++) {
		if (certain(i)) {
			sure[i] = true;

			for (int j = 0; j < dead[i].size(); j++) {
				if (!dead[i][j]) {
					auto it = set.find('a' + j);
					assert(it != set.end());
					set.erase(it);
				}
			}
		}
	}

	for (int i = 0; i < word_size; i++) {
		if (!sure[i]) {
			int counter = 0;

			for (char c : set) {
				if (!dead[i][c - 'a']) {
					counter++;
				}
			}

			if (counter == 1) {
				changed = true;

				for (char c : set) {
					if (!dead[i][c - 'a']) {
						killAllButOne(i, c);
					}
				}
			}
		}
	}

	return changed;
}

bool DeadLetters::certain(int i) {
	return dead[i].count() + 1 == dead[i].size();
}

vector<char> DeadLetters::certainLetters() {
	vector<char> res;

	for(int i = 0; i < word_size; i++) {
		if (certain(i)) {
			for (int j = 0; j < dead[i].size(); j++) {
				if (!dead[i][j])
					res.emplace_back('a' + j);
			}
		}
	}

	return res;
}

int DeadLetters::coherent_guesses(int layer, vector<char> guess) {
	if (layer == word_size) {
		vector<char> inter = intersect(guess, known);

		return inter == known;
	}

	int res = 0;

	for (size_t i = 0; i < 26; i++) {
		if (!dead[layer][i]) {
			vector<char> next = guess;
			next.emplace_back('a' + i);

			res += coherent_guesses(layer + 1, next);
		}
	}

	return res;
}

vector<char> DeadLetters::intersect(vector<char> a, vector<char> b) {
	vector<char> res;

	sort(a.begin(), a.end());
	sort(b.begin(), b.end());

	size_t i = 0, j = 0;
	while(i < a.size() && j < b.size()) {
		if (a[i] < b[j]) {
			i++;
		} else if (a[i] > b[j]) {
			j++;
		} else {
			res.emplace_back(a[i]);
			i++;
			j++;
		}
	}

	return res;
}


