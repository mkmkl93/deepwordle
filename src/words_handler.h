#ifndef WORDLE_WORDS_HANDLER_H
#define WORDLE_WORDS_HANDLER_H

#include <cstdio>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

class WordsHandler {
public:
	static vector< string > solutions();
	static vector< string > guesses();
private:
	// read a file of words into a vector
	static vector< string > readWordFile(const string &fn);
};

#endif //WORDLE_WORDS_HANDLER_H
