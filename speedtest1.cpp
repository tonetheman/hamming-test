#include <iostream>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <polarssl/sha1.h>

using namespace std;

const int WORD_COUNT = 12;
const int POPULATION_SIZE = 3;
const int CUTOFF = 1000000;

typedef vector<string> VS;
typedef vector<int> VI;

int make_rand() {
	return abs(rand()) % 1000;
}

struct Words {
	VS words;
	void init(string words_filename) {
		words = load_file(words_filename);
	}
	int size() {
		return words.size();
	}
	VS load_file(string words_filename) {
		ifstream inf;
		//inf.open("./words");
		inf.open(words_filename.c_str());
		if (!inf) {
			cout << "unable to open word list" << endl;
			exit(1);
		}
		string buffer;
		VS result;
		while(inf >> buffer) {
			result.push_back(buffer);
		}
		inf.close();
		return result;
	}

	string get_random_word() {
		return words[make_rand()];
	}

	string make_target_string() {
		string ts;
		for(int i=0;i<WORD_COUNT;i++) {
			ts += get_random_word();
			if (i==WORD_COUNT-1) {
				// do nothing
			} else {
				ts += " ";
			}	
		}
		return ts;
	}
	string get(int index) {
		return words[index];
	}
};

ostream& operator<<(ostream& os, VS& vs) {
	VS::iterator ip = vs.begin();
	while(ip!=vs.end()) {
		os << *ip++ << " ";
	}
	return os;
}


void init() {
	srand(time(0));
}

int main(int argc, char* argv[]) {

	string target_filename;
	string words_filename;
	for(int i=1;i<argc;i++) {
		if (strcmp("-t",argv[i])==0) {
			target_filename = argv[++i];
		}
		if (strcmp("-w",argv[i])==0) {
			words_filename = argv[++i];
		}
	}

	init();

	Words words;
	words.init("words");

	return 0;
}
