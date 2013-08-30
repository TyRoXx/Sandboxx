all : main.s a.out

a.out : main.cpp
	g++ -std=c++0x -Wall -Wextra -pedantic main.cpp

main.s : main.cpp
	g++ -std=c++0x -O3 -S main.cpp
