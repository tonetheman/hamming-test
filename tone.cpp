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
const int CUTOFF = 2;

typedef vector<string> VS;
typedef vector<int> VI;


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

ostream& operator<<(ostream& os, VS& vs) {
	VS::iterator ip = vs.begin();
	while(ip!=vs.end()) {
		os << *ip++ << " ";
	}
	return os;
}

int make_rand();

struct Population {
	VS population[POPULATION_SIZE];

	void init(VS& words) {
		for(int i=0;i<POPULATION_SIZE;i++) {
			for(int j=0;j<WORD_COUNT;j++) {
				int choice = make_rand();
				population[i].push_back(words[choice]);
			}	
		}
	}

	VS get(int i) {
		return population[i];
	}

	void make_string(VS words, char * buffer) {
		int cp=0;
		
		for(int i=0;i<WORD_COUNT;i++) {
			strcpy(buffer+cp,words[i].c_str());
			cp += strlen(words[i].c_str());
			if (i!=WORD_COUNT-1) {
				*(buffer+cp) = ' ';
			}
			cp++;
		}
	}

//	void score_population(SimData& sim, int * pop_score) {
	void score_population(SHA target_sha, int * pop_score) {
		char buffer[1024]; // big enough to hold 12 words

	
		for(int i=0;i<POPULATION_SIZE;i++) {
			memset(buffer,'\0',1024);
			make_string(population[i], buffer);
			SHA tmp_sha = SHA::sha_it(buffer);
			int tmp_sha_score = SHA::score(tmp_sha, target_sha);
			pop_score[i] = tmp_sha_score;
		}
	}
};

struct Words {
	VS words;
	void init() {
		words = load_file();
	}
	int size() {
		return words.size();
	}
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

	string get_random_word(VS& vs) {
		return vs[make_rand()];
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
		return SHA::sha_it(s.c_str());
	}
};

struct SimData {
	int step;
	string target;
	SHA target_sha;
	Words words;

	int current_low_index;
	VS current_low;
	int current_low_score;

	SimData() : step(0) {}

	void load_data() {
		words.load_file();
	}
};

int make_rand() {
	return rand() % 1000;
}

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

void make_string(VS words, char * buffer) {
	int cp=0;
	
	for(int i=0;i<WORD_COUNT;i++) {
		strcpy(buffer+cp,words[i].c_str());
		cp += strlen(words[i].c_str());
		if (i!=WORD_COUNT-1) {
			*(buffer+cp) = ' ';
		}
		cp++;
	}
}

void report_score(int * pop_score) {
	for(int i=0;i<POPULATION_SIZE;i++) {
		cout << "score: " << dec << i << " " << pop_score[i] << endl;
	}
}

int find_low(int* pop_score) {
	int index = -1;
	int low_score = 10000;
	for(int i=0;i<POPULATION_SIZE;i++) {
		if (pop_score[i] < low_score) {
			index = i;
			low_score = pop_score[i];
		}
	}
	return index;
}

void setup_new_population(SimData& sim) {
	VS good_stuff = population[sim.current_low_index];

	for(int i=0;i<POPULATION_SIZE;i++) {
		population[i].clear();	
	}
	initialize_population(sim.words);
}

void write_current_low(SimData& sim) {
	ofstream outf;
	outf.open("./results", ios::app);
	VS::iterator it = sim.current_low.begin();
	while(it!=sim.current_low.end()) {
		outf << *it;
		it++;
	}
	outf << endl;
	outf << "low score: " << sim.current_low_score << endl;	
	outf.close();
}

void sim_loop(SimData& sim) {
	int pop_score[POPULATION_SIZE];
	int lowest_score = 1024;
	int count = 0;
	initialize_population(sim.words);

	while(true) {
		//cout << "scoring current population" << endl;
		score_population(sim, pop_score);
		//cout << "reporing score" << endl;
		//report_score(pop_score);
		//cout << "---------------" << endl;
		int new_low_index = find_low(pop_score);

		if (new_low_index<lowest_score) {
			lowest_score = pop_score[sim.current_low_index];
			sim.current_low_index = new_low_index;
			sim.current_low = population[sim.current_low_index];
			sim.current_low_score = pop_score[sim.current_low_index];
			write_current_low(sim);
		}

		setup_new_population(sim);

		count++;
		if (count>CUTOFF) break;
	}

	cout << "final low is " << sim.current_low << endl;
	cout << "score for final low is " << sim.current_low_score << endl;
}

int main() {
	init();
	SimData sim;
	sim.load_data();

	cout << "total number of words loaded: " << sim.words.size() << endl;
	cout << "setting up random population..." << endl;

	string target = make_target_string(sim.words);
	cout << "target is set to: " << target << endl;
	SHA target_sha = make_target_sha(target);
	cout << "target sha is: " << target_sha << endl;
	sim.target = target;
	sim.target_sha = target_sha;

	sim_loop(sim);

	return 0;
}
