CXXFLAGS=-std=c++11 -Wpedantic -Wall -Wextra -Werror -Wzero-as-null-pointer-constant
CC=g++ -ggdb

compile: main.cpp VMManager.cpp
	$(CC) $(CXXFLAGS) -o icsosvm main.cpp VMManager.cpp -I.

clean:
	/bin/rm icsosvm output-dp.txt core.* vgcore.*