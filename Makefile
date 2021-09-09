all: ct
ct: main.o
	gcc main.o -o ct
main.o: main.c
	gcc  -c main.c -o main.o