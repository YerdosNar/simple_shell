CC	:= gcc
CFLAGS  := -Wall -Wextra -Wpedantic -O2 -g
SRC 	:= src/main.c src/input.c
OBJ 	:= $(SRC:.c=.o)
TARGET  := shell

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f shell src/*.o

.PHONY: clean
