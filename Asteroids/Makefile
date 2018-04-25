SRC = $(wildcard *.cpp)
OBJECTS = $(SRC:.cpp=.o)
PROGNAME = asteroids_game
LINKS = -lsfml-audio -lsfml-graphics -lsfml-system -lsfml-window

all: $(OBJECTS)
	clang++ -std=c++11 -stdlib=libc++ -o $(PROGNAME) $(OBJECTS) $(LINKS)
run:
	@./$(PROGNAME)	
%.o: %.cpp include/%.hpp
	clang++ -std=c++11 -stdlib=libc++ -o $@ -c $<

clean:
	rm *.o $(PROGNAME)