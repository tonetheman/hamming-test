

tone: tone.cpp
	g++ -o tone tone.cpp -I polarssl-0.11.1/include \
-L polarssl-0.11.1/library -lpolarssl
