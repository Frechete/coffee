main: main.cpp
	g++ main.cpp -o main -g -std=c++11 -Wall -lpthread

Debug: main

clean: rm main

check: ./main data.txt

