#include "response.h"

Response::Response(size_t size, int n) {
	code.resize(size);

	for (int i = size - 1; i >= 0; i--) {
		code[i] = n % 3 + '0';
		n /= 3;
	}
}

Response::operator int() {
	int ret = 0;

	for (char &c : code) {
		ret *= 3;
		ret += c - '0';
	}

	return ret;
}

string Response::pretty_string() {
	vector<string> color{"⬜", "\xF0\x9F\x9F\xA8", "\xF0\x9F\x9F\xA9"};
	string s;

	for(char &c : code)
		s += color[c - '0'];

	return s;
}

string Response::latex_string() {
	vector<string> color{"\\w", "\\y", "\\g"};
	string s;

	for(char &c : code)
		s += color[c - '0'];

	return s;
}
