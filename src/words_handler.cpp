#include "words_handler.h"

vector< string > WordsHandler::solutions() {
	string filename = "../wordle solutions sorted.txt";

	return readWordFile(filename);
}

vector<string> WordsHandler::guesses() {
	string filename = "../wordle solutions sorted.txt";

	return readWordFile(filename);
}

vector<string> WordsHandler::readWordFile(const string &fn) {
	vector< string > words;

	ifstream wordFile(fn);
	string w;

	while (getline(wordFile, w)) {
		words.push_back(w);
	}

	wordFile.close();

	return words;
}
