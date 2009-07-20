#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct LowInformation {
	string lowstring;
	int lowscore;
};

ostream& operator<<(ostream& os, LowInformation src) {
	os << src.lowstring << " " << src.lowscore;
	return os;
}

LowInformation load_saved_low_from_file(string savedlow_filename) {
	ifstream inf;
        inf.open(savedlow_filename.c_str());
	LowInformation l;
	char buffer[1024];
	inf.getline(buffer,1024);
	l.lowstring = buffer;
        inf >> l.lowscore;
        inf.close();
	return l;
}


int main() {

	cout << load_saved_low_from_file("./savedlow") << endl;
	return 0;
}
