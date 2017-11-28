main: main.c
	gcc -o main main.c -pthread -Wall -Werror -std=c99
	
run: main
	./main 10 11 3 7
