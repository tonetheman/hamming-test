#include <iostream>
#include <fstream>
#include <string>
#include <polarssl/sha1.h>

using namespace std;

int main() {

	const int COUNT=10000000;

	unsigned char sha1sum[20];

	char * cp = "this is a long but constant update";

	int start_time = time(0);
	for(int i=0;i<COUNT;i++) {
		sha1_context ctx;
		sha1_starts(&ctx);
		sha1_update(&ctx,(unsigned char*)cp,strlen(cp));
		sha1_finish(&ctx, sha1sum);	
	}
	int stop_time = time(0);

	cout << "seconds: " << stop_time-start_time << endl;
	cout << "total number of sha count: " << COUNT << endl;

	return 0;
}
