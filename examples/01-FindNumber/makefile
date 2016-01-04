.PHONY : reset clean example start

all: reset example clean start

reset : 
	@reset
	@echo '**** Compiling example ****'
	
clean :
	@echo "[Info] Cleaning object and temp files"
	@rm -f *.o *~
	
example :
	@g++ -std=c++11 -Wall -O3 -o example main.cpp
	
start :
	@./example
