#include "response.h"

Response::Response(int c) {
	int m = (int)pow(3, WORD_SIZE - 1);
	for (int i = 0; i < WORD_SIZE; i++) {
		code[i] = (c / m) % 3;
		m /= 3;
	}
}

Response::Response(string s) {
	for(int i = 0; i < WORD_SIZE; i++) {
		code[i] = s[i] - '0';
	}
}