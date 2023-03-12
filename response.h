#ifndef WORDLE_RESPONSE_H
#define WORDLE_RESPONSE_H

#include <valarray>

using namespace std;

const int ALPHABET_SIZE = 26;
const int WORD_SIZE = 5;

struct Response {
	array<int, WORD_SIZE> code;

	Response() {};

	Response(int c);

	Response(string s);
};

#endif //WORDLE_RESPONSE_H
