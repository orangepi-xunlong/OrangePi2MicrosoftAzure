CC= gcc
FLAGS=
TARGET= update

OBJ= Audio2Command.o

all: $(TARGET)

$(TARGET): $(OBJ)
	@$(CC) $(FLAGS) -o $@ $^

Audio2Command.o: Audio2Command.c
	@$(CC) $(FLAGS) -c -o $@ $^

clean:
	@rm *.o $(TARGET)

.PHONY: all clean
