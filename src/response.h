#ifndef WORDLE_RESPONSE_H
#define WORDLE_RESPONSE_H

#include <valarray>

using namespace std;

struct Response {
	string code;

	Response() = default;

	Response(int size, int n);

	explicit Response(string &&s) : code(s) {};

	explicit operator int();

	string pretty_string();

	string latex_string();
};

#endif //WORDLE_RESPONSE_H
