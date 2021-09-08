all: tester

tester: main.o
	gcc main.o -o tester
main.o: main.c
	gcc  -c main.c -o main.o