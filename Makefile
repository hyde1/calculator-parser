CC=gcc
FLAGS=-std=c99 -lm
EXEC=calc

all: $(EXEC)

$(EXEC):
	$(CC) $(FLAGS) -o $(EXEC) main.c functions.c

debug: FLAGS+= -DDEBUG=1
debug: $(EXEC)

clean:
	rm -rf $(EXEC)
