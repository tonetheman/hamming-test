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
		dbg("enter make_target_string");
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
	SHA make_target_sha(const string& s) {
		return SHA::sha_it(s.c_str());
	}
};

ostream& operator<<(ostream& os, VS& vs) {
	VS::iterator ip = vs.begin();
	while(ip!=vs.end()) {
		os << *ip++ << " ";
	}
	return os;
}


struct Population {
	VS population[POPULATION_SIZE];

	void init(Words& words) {
		for(int i=0;i<POPULATION_SIZE;i++) {
			for(int j=0;j<WORD_COUNT;j++) {
				int choice = make_rand();
				population[i].push_back(words.get(choice));
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

	void clear() {
		for(int i=0;i<POPULATION_SIZE;i++) {
			population[i].clear();
		}
	}

};

struct SimData {
	int step;
	string target;
	SHA target_sha;
	Words words;
	Population population;

	int current_low_index;
	VS current_low;
	int current_low_score;

	SimData() : step(0) {}

	void init(string words_filename) {
		words.init(words_filename);
	}

	void load_data(string words_filename) {
		words.load_file(words_filename);
	}

	string set_target_string(string target_filename) {
	}

	string make_target_string() {
		dbg("SimData: enter make_target_string");
		target = words.make_target_string();
		dbg("SimData: exit make_target_string");
		return target;
	}
	SHA make_target_sha() {
		target_sha = words.make_target_sha(target);
		return target_sha;
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


	void sim_loop() {
		int pop_score[POPULATION_SIZE];
		int lowest_score = 1024;
		int count = 0;
		population.init(words);

		while(true) {

			population.score_population(target_sha, pop_score);
			int new_low_index = find_low(pop_score);
			int new_low_score = pop_score[new_low_index];

			if (new_low_score<lowest_score) {
				lowest_score = new_low_score;
				this->current_low_index = new_low_index;
				this->current_low = population.get(new_low_index);
				this->current_low_score = new_low_score;
				write_current_low();
			}

			population.clear();
			population.init(words);

			count++;
			if (count>CUTOFF) break;
		}
	}

	void write_current_low() {
		ofstream outf;
		outf.open("./results", ios::app);
		VS::iterator it = current_low.begin();
		while(it!=current_low.end()) {
			outf << *it << " ";
			it++;
		}
		outf << endl;
		outf << "low score: " << current_low_score << endl;	
		outf.close();
	}

};

ostream& operator<<(ostream& os, SHA& sha) {
	char buffer[4];
	for(int i=0;i<20;i++) {
		sprintf(buffer, "%02x", (int)sha.sha1sum[i]);
		os << buffer;
	}
	return os;
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

int main(int argc, char* argv[]) {

	if (argc!=5) {
		cout << "unable to parse args" << endl;
		cout << "tone -t target_file -w word_file" << endl;
		return -1;
	}

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

	dbg("create sim");
	SimData sim;
	sim.init(words_filename);
	sim.load_data(words_filename);
	dbg("after load data");

	string target = sim.set_target_string(target_filename);
	//string target = sim.make_target_string();
	dbg("after make target string");

	SHA target_sha = sim.make_target_sha();
	dbg("after target sha make");

	sim.sim_loop();
	dbg("after sim loop");

	return 0;
}
