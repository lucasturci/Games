SRC = $(wildcard *.c)
OBJECTS = $(SRC:.c=.o)
PROGNAME = snake_game
LINKS = -lallegro_main -lallegro_primitives -lallegro_font -lallegro_ttf  -lallegro

all: $(OBJECTS)
	@gcc -o $(PROGNAME) $(OBJECTS) $(LINKS)
run: $(PROGNAME)
	@./$(PROGNAME)	
%.o: %.c
	@gcc -o $@ -c $< -I./
clean:
	rm *.o $(PROGNAME)