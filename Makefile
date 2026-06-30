CFLAGS = -Wall -Wextra -Wpedantic -O2 -g

shell: main.o input.o
	gcc main.o input.o -o shell

main.o: src/main.c include/input.h
	gcc -c src/main.c -o main.o

input.o: src/input.c include/input.h
	gcc -c src/input.c -o input.o

.PHONY: clean
clean:
	rm -f shell *.o
