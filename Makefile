all: ct
ct: main.o
	gcc main.o -o ct
main.o: main.c
	gcc  -c main.c -o main.o
testing:
	gcc ./testing_program/main.c -o a.out
clean:
	rm main.o