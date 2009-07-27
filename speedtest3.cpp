#include <iostream>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <polarssl/sha1.h>

/*
this version changes the way that words are read in
uses a more c like structure
*/

using namespace std;

const int TOTAL_WORDS = 1000;
typedef char* PCHAR;
static PCHAR words[TOTAL_WORDS]; // compilatio unit scope
static PCHAR words_space[TOTAL_WORDS];
static int words_len[TOTAL_WORDS];
static int words_space_len[TOTAL_WORDS];

const int WORD_COUNT = 12;
int THRESHOLD = 50; // score threshold
int SIM_COUNT = 10000000;
const int SHA_SIZE = 20;

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
	unsigned char sha1[SHA_SIZE];
};

// GLOBAL target value, filled out in main
static HashWordListTuple target_tuple;

ostream& operator<<(ostream& os, HashWordListTuple& src) {
	for(int i=0;i<WORD_COUNT;i++) {
		int idx = src.wl.wl[i];
		if (i==WORD_COUNT-1) os << words[idx];
		else os << words_space[idx];
	}
	os << ": ";
	for(int i=0;i<SHA_SIZE;i++) {
		char buffer[10];
		sprintf(buffer, "%02x", src.sha1[i]);
		os << buffer;
	}
	return os;
}

struct Words {

	void init(string words_filename) {
		load_file(words_filename);
	}
	void load_file(string words_filename) {
		ifstream inf;
		inf.open(words_filename.c_str());
		if (!inf) {
			cout << "unable to open word list" << endl;
			exit(1);
		}
		string buffer;
		int index= 0;
		while(inf >> buffer) {
			words[index] = new char[buffer.size()+1];
			strcpy(words[index],buffer.c_str());
			words_space[index] = new char[buffer.size()+2];
			strcpy(words_space[index], words[index]);
			strcat(words_space[index], " ");

			words_len[index] = strlen(words[index]);
			words_space_len[index] = strlen(words_space[index]);

			index++;
		}
		inf.close();
	}

	void choose(HashWordListTuple& ht) {
		ht.wl.init();
		sha1_context ctx;
		sha1_starts(&ctx);
		for(int i=0;i<WORD_COUNT;i++) {
			int fix = ht.wl.wl[i];
			if (i==WORD_COUNT-1) {
				const char * cp = words_space[fix]; 
				sha1_update(&ctx,(unsigned char*)cp,words_space_len[fix]);
			} else {
				const char * cp = words[fix];
				sha1_update(&ctx,(unsigned char*)cp,words_len[fix]);
			}
		}
		sha1_finish(&ctx,ht.sha1);
	}

	HashWordListTuple choose() {
		char SPACE[2]; SPACE[0] = ' '; SPACE[1] = 0;
		HashWordListTuple ht;
		ht.wl.init(); // this makes a random choice from all words
		sha1_context ctx;
		sha1_starts(&ctx);
		for(int i=0;i<WORD_COUNT;i++) {
			int fix = ht.wl.wl[i];
			if (i==WORD_COUNT-1) {
				const char * cp = words_space[fix]; 
				sha1_update(&ctx,(unsigned char*)cp,words_space_len[fix]);
			} else {
				const char * cp = words[ht.wl.wl[i]];
				sha1_update(&ctx,(unsigned char*)cp,words_len[fix]);
			}
		}
		sha1_finish(&ctx,ht.sha1);
		return ht;
	}

};

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

static int score(HashWordListTuple& s2) {
                unsigned char result[SHA_SIZE];
                int sum = 0;
                for(int i=0;i<SHA_SIZE;i++ ) {
                        result[i] = target_tuple.sha1[i] ^ s2.sha1[i];
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

	// fill out global target
	const char * real_target =  "I would much rather hear more about your whittling project";
        sha1_context target_ctx;
        sha1_starts(&target_ctx);
	sha1_update(&target_ctx,(unsigned char*)
		real_target,strlen(real_target));
	sha1_finish(&target_ctx, target_tuple.sha1);


	HashWordListTuple ht;
	for(int i=0;i<SIM_COUNT;i++) {
		words.choose(ht);
		//cout << ht << endl;
		int scr = score(ht);
		if (scr<THRESHOLD) {
			cout << scr << endl;
		}
	}

	return 0;
}
