
all: tone test_it

test_it : test_it.cpp
	g++ -g -o test_it test_it.cpp -I polarssl-0.11.1/include \
-L polarssl-0.11.1/library -lpolarssl

tone: tone.cpp
	g++ -g -o tone tone.cpp -I polarssl-0.11.1/include \
-L polarssl-0.11.1/library -lpolarssl
