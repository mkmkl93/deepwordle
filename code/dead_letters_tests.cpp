#include "dead_letters.h"
#include <cassert>

const int WORD_SIZE = 5;

void check(const DeadLetters &d, const vector<int> &v, const vector<char> &m) {
	for (int i = 0; i < WORD_SIZE; i++) {
		assert(d.dead[i] == v[i]);
	}

	assert(d.known == m);
}

int main() {
	{
		DeadLetters d(WORD_SIZE);
		Response r("00000");
		d.addResponse("aaaaa", r);
		check(d, {0b1, 0b1, 0b1, 0b1, 0b1}, {});
	}

	{
		DeadLetters d(WORD_SIZE);
		Response r("20000");
		d.addResponse("abcde", r);
		check(d, {0b11111111111111111111111110, 0b11110, 0b11110, 0b11110, 0b11110}, {'a'});
	}

	{
		DeadLetters d(WORD_SIZE);
		Response r("20000");
		d.addResponse("aacde", r);
		check(d, {0b11111111111111111111111110, 0b11101, 0b11101, 0b11101, 0b11101}, {'a'});
	}

	{
		DeadLetters d(WORD_SIZE);
		Response r("21000");
		d.addResponse("aaabc", r);
		check(d, {0b11111111111111111111111110, 0b111, 0b111, 0b110, 0b110}, {'a', 'a'});
	}

	{
		DeadLetters d(WORD_SIZE);
		Response r("11000");
		d.addResponse("aaabc", r);
		check(d, {0b111, 0b111, 0b111, 0b110, 0b110}, {'a', 'a'});
	}

	{
		DeadLetters d(WORD_SIZE);
		Response r("11000");
		d.addResponse("aabbb", r);
		check(d, {0b11, 0b11, 0b10, 0b10, 0b10}, {'a', 'a'});
	}

	{
		DeadLetters d(WORD_SIZE);
		Response r;

		r = Response("11000");
		d.addResponse("aabbb", r);
		check(d, {0b11, 0b11, 0b10, 0b10, 0b10}, {'a', 'a'});

		r = Response("10000");
		d.addResponse("abbbb", r);
		check(d, {0b11, 0b11, 0b10, 0b10, 0b10}, {'a', 'a'});
	}

	{
		DeadLetters d(WORD_SIZE);
		Response r;

		r = Response("10000");
		d.addResponse("abbbb", r);
		check(d, {0b11, 0b10, 0b10, 0b10, 0b10}, {'a'});

		r = Response("11000");
		d.addResponse("aabbb", r);
		check(d, {0b11, 0b11, 0b10, 0b10, 0b10}, {'a', 'a'});
	}

	{
		DeadLetters d(WORD_SIZE);
		Response r;

		r = Response("11000");
		d.addResponse("abddd", r);
		check(d, {0b1001, 0b1010, 0b1000, 0b1000, 0b1000}, {'a', 'b'});

		r = Response("11000");
		d.addResponse("acddd", r);
		check(d, {0b1001, 0b1110, 0b1000, 0b1000, 0b1000}, {'a', 'b', 'c'});
	}
}
