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
int THRESHOLD = 50; // score threshold
int SIM_COUNT = 10000000;

typedef vector<string> VS;
typedef vector<int> VI;

int make_rand() {
	return abs(rand()) % 1000;
}

struct WordIndexList {
	int wl[WORD_COUNT];
	void init() {
		for(int i=0;i<WORD_COUNT;i++) wl[i] = make_rand();
	}
	void copy(WordIndexList& src) {
		for(int i=0;i<WORD_COUNT;i++) wl[i] = src.wl[i];
	}
};

struct HashWordListTuple {
	WordIndexList wl;
	unsigned char sha1[20];
};

ostream& operator<<(ostream& os, HashWordListTuple& src) {
	for(int i=0;i<20;i++) {
		char buffer[10];
		sprintf(buffer, "%02x", src.sha1[i]);
		os << buffer;
	}
	return os;
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

	HashWordListTuple choose() {
		char SPACE[2]; SPACE[0] = ' '; SPACE[1] = 0;
		HashWordListTuple ht;
		ht.wl.init(); // this makes a random choice from all words
		sha1_context ctx;
		sha1_starts(&ctx);
		for(int i=0;i<WORD_COUNT;i++) {
			const char * cp = words[ht.wl.wl[i]].c_str(); // not safe really
			sha1_update(&ctx,(unsigned char*)cp,strlen(cp));
			if (i!=WORD_COUNT-1) { 
				sha1_update(&ctx, (unsigned char*)SPACE,1);
			}
		}
		sha1_finish(&ctx,ht.sha1);
		return ht;
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


static int __score(unsigned char c) {
                int sum = 0;
                switch(c) {
                        case 0x1:
                        case 0x2: case 0x4: case 0x8:
                                sum +=1; break;
                        case 0x3: case 0x5: case 0x6: case 0x9:
                        case 10: case 12:
                                sum +=2; break;
                        case 0x7: case 14: case 13: case 11:
                                sum += 3; break;
                        case 15:
                                sum +=4; break;
                        }
                return sum;
        }

static int score(HashWordListTuple& s1, HashWordListTuple& s2) {
                unsigned char result[20];
                int sum = 0;
                for(int i=0;i<20;i++ ) {
                        result[i] = s1.sha1[i] ^ s2.sha1[i];
                        unsigned char part1 = result[i] >> 4;
                        unsigned char part2 = (unsigned char)(result[i] & 0x0f);
                        sum += __score(part1);
                        sum += __score(part2);
                }
                return sum;
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

	const char * real_target =  "I would much rather hear more about your whittling project";
        sha1_context target_ctx;
        sha1_starts(&target_ctx);
	sha1_update(&target_ctx,(unsigned char*)
		real_target,strlen(real_target));
	HashWordListTuple target_tuple;
	sha1_finish(&target_ctx, target_tuple.sha1);


	for(int i=0;i<SIM_COUNT;i++) {
		HashWordListTuple ht =words.choose();
		int scr = score(target_tuple,ht);
		if (scr<THRESHOLD) {
			cout << scr << endl;
		}
	}

	return 0;
}
