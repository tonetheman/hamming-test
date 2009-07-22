
all: tone test_it junk speedtest

INC=-I polarssl-0.11.1/include
LIB=-L polarssl-0.11.1/library -lpolarssl

speedtest : speedtest.cpp
	g++ -g -o speedtest speedtest.cpp ${INC} ${LIB}

junk : junk.cpp
	g++ -g -o junk junk.cpp

test_it : test_it.cpp
	g++ -g -o test_it test_it.cpp ${INC} ${LIB}

tone: tone.cpp
	g++ -g -o tone tone.cpp ${INC} ${LIB}
