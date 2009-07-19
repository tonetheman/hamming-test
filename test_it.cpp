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
const int POPULATION_SIZE = 3;
const int CUTOFF = 200000;

typedef vector<string> VS;
typedef vector<int> VI;

void dbg(string s) {
	cout << "dbg: " << s << endl;
}

int make_rand() {
	return rand() % 1000;
}

struct SHA {
	public:
		unsigned char sha1sum[20];

	static SHA sha_it(const char * cp) {
		sha1_context ctx;
		sha1_starts(&ctx);
		sha1_update(&ctx,(unsigned char*)cp, strlen(cp));
		SHA s;
		sha1_finish(&ctx, s.sha1sum);
		return s;
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

	static int score(SHA s1, SHA s2) {
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
};

ostream& operator<<(ostream& os, SHA& sha) {
	char buffer[4];
	for(int i=0;i<20;i++) {
		sprintf(buffer,"%02x",(int)sha.sha1sum[i]);
		os << buffer;
	}
	return os;
}

void test() {
	const char * cp = "Rubinius one eight six active active record memcached exception JRuby DHH TOKYO sdfe3";
	SHA sha = SHA::sha_it(cp);
	cout << sha << endl;

	const char * cp1 = "I am not a big believer in fortune telling";
	SHA sha1 = SHA::sha_it(cp1);
	cout << sha1 << endl;

	// hamming testing
	cout << "score: " << dec << SHA::score(sha,sha1) << endl;
}

void test2() {
	const char * cp = "What you write today will become legacy";
	SHA sha = SHA::sha_it(cp);
	cout << sha << endl;
	const char * cp2 = "RuBy one eight six rspec mongrel MRI jruby jruby memcached exception reflection utf8E";
	SHA sha2 = SHA::sha_it(cp2);
	cout << sha2 << endl;

	int score = SHA::score(sha,sha2);
	cout << "score is " << score << endl;
}

int main() {

	test();
	test2();

	return 0;
}
