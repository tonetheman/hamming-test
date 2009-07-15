#include <iostream>
#include <cstring>
#include <polarssl/sha1.h>

using namespace std;

class SHA {
	public:
		unsigned char sha1sum[20];
};

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

int main() {
	test();
	return 0;
}
