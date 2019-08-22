SRC = $(wildcard *.c)
OBJECTS = $(SRC:.c=.o)
PROGNAME = snake_game
LINKS = `pkg-config allegro-5 allegro_primitives-5 allegro_font-5 allegro_ttf-5 --libs --cflags`

all: $(OBJECTS)
	@gcc -o $(PROGNAME) $(OBJECTS) $(LINKS)
run: $(PROGNAME)
	@./$(PROGNAME)	
%.o: %.c
	@gcc -o $@ -c $< -I./
clean:
	rm *.o $(PROGNAME)