SRC = $(wildcard *.c)
OBJECTS = $(SRC:.c=.o)
PROGNAME = tetris_game
LINKS = `pkg-config allegro-5 allegro_font-5 allegro_ttf-5 allegro_image-5 allegro_audio-5 allegro_acodec-5 allegro_primitives-5 --libs --cflags`

all: $(OBJECTS)
	gcc -o $(PROGNAME) $(OBJECTS) $(LINKS) 
run:
	@./$(PROGNAME)	
%.o: %.c
	@gcc -o $@ -c $< -I./