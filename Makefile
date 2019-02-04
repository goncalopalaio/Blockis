# Flags
CFLAGS = -v -g -std=c++11 -Wall -pedantic
LIBS := -lglfw -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lm -lGLEW -L/usr/local/lib
CC= g++
OBJS =

game: 
	$(CC) $(CFLAGS) $(OBJS) -o game game.cpp $(LIBS)
clean:
	@rm game