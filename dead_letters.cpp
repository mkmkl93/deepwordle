#include "dead_letters.h"

void DeadLetters::killOne(int i, char letter) {
	dead[i][letter - 'a'] = true;
}

void DeadLetters::killAllButOne(int i, char letter) {
	dead[i].set();
	dead[i][letter - 'a'] = false;
}

void DeadLetters::addResponse(string w, Response r) {
	auto &code = r.code;

	vector<char> v_known;

	for (int i = 0; i < word_size; i++) {
		if (code[i] == 0) {
			bool hasMultiple = false, allOtherCorrect = true, allOtherFalse = true;
			for (int j = 0; j < word_size; ++j) {
				if (i != j && w[i] == w[j]) {
					hasMultiple = true;
					allOtherCorrect &= code[j] == 2;
					allOtherFalse &= code[j] == 0;
				}
			}
			if (hasMultiple) {
				// TODO błąd w głównej pracy
				if (allOtherCorrect) {
					for (int j = 0; j < word_size; j++) {
						if (w[j] != w[i] || code[j] == 0) {
							killOne(j, w[i]);
						}
					}
				} if (allOtherFalse) {
					for (int j = 0; j < 5; ++j)
						killOne(j, w[i]);
				} else {
					killOne(i, w[i]);
				}
			} else {
				for (int j = 0; j < 5; ++j)
					killOne(j, w[i]);
			}
		} else if (code[i] == 1) {
			v_known.emplace_back(w[i]);
			// TODO może pomijanie tego sprawiłoby by więcej stanów było takich samych? np "abbbb" i "acccc" i "a****" byłyby tym samym po próbie "*a***" i odpowiedzi "01000"
			killOne(i, w[i]);
		} else {
			v_known.emplace_back(w[i]);
			killAllButOne(i, w[i]);
		}
	}

	unionKnown(v_known);
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
		res.emplace_back(v_known[j]);
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
