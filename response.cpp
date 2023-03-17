#include "response.h"

Response::Response(size_t size, int n) {
	code.resize(size);

	for (char &c : code) {
		c = n % 3;
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