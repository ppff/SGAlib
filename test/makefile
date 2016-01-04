.PHONY : reset clean test start

all: reset test clean start

reset : 
	@reset
	@echo '**** Compiling test ****'
	
clean :
	@echo "[Info] Cleaning object and temp files"
	@rm -f *.o *~
	
example :
	@g++ -std=c++11 -Wall -g -o test main.cpp
	
start :
	@./test
