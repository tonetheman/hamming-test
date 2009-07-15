#include <iostream>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstdlib>
#include <polarssl/sha1.h>

using namespace std;

const int WORD_COUNT = 12;

typedef vector<string> VS;

VS load_file() {
	ifstream inf;
	inf.open("./words");
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

int make_rand() {
	return rand() % 1000;
}
string get_random_word(VS& vs) {
	return vs[make_rand()];
}

class SHA {
	public:
		unsigned char sha1sum[20];
};

ostream& operator<<(ostream& os, SHA& sha) {
	for(int i=0;i<20;i++) {
		os << hex << (int)sha.sha1sum[i];
	}
	return os;
}

SHA sha_it(const char * cp) {
	sha1_context ctx;
	sha1_starts(&ctx);
	sha1_update(&ctx,(unsigned char*)cp, strlen(cp));
	SHA s;
	sha1_finish(&ctx, s.sha1sum);
	return s;
}

void sha_print(SHA s) {
	for(int i=0;i<20;i++) {
		cout << hex << (int)s.sha1sum[i];
	}
	cout << endl;
}

int __score(unsigned char c) {
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

int score(SHA s1, SHA s2) {
	SHA result;
	int sum = 0;
	for(int i=0;i<20;i++ ) {
		result.sha1sum[i] = s1.sha1sum[i] ^ s2.sha1sum[i];
		unsigned char part1 = result.sha1sum[i] >> 4;
		unsigned char part2 = (unsigned char)(result.sha1sum[i] & 0x0f);
		sum += __score(part1);
		sum += __score(part2);
	}
	return sum;
}

void test() {
	const char * cp = "Rubinius one eight six active active record memcached exception JRuby DHH TOKYO sdfe3";
	SHA sha = sha_it(cp);
	sha_print(sha);

	const char * cp1 = "I am not a big believer in fortune telling";
	SHA sha1 = sha_it(cp1);
	sha_print(sha1);

	// hamming testing
	cout << "score: " << dec << score(sha,sha1) << endl;
}

void init() {
	srand(0);
}

string make_target_string(VS& words) {
	string ts;
	for(int i=0;i<WORD_COUNT;i++) {
		ts += get_random_word(words);
		if (i==WORD_COUNT-1) {
			// do nothing
		} else {
			ts += " ";
		}	
	}
	return ts;
}

SHA make_target_sha(const string& s) {
	return sha_it(s.c_str());
}

int main() {
	init();
	VS words = load_file();
	cout << "total number of words loaded: " << words.size() << endl;
	string target = make_target_string(words);
	cout << "target is set to: " << target << endl;
	SHA target_sha = make_target_sha(target);
	cout << "target sha is: " << target_sha << endl;
	//test();
	return 0;
}
