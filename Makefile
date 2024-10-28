#------------------------------------------------------------------------------
#  Makefile for Synthesis Shader by Noel Ball
#  Version 0.1
#
#  make                     Compiles Synthesis Shader
#
#  make clean		    Removes compiled executable
#------------------------------------------------------------------------------

MAIN           = Synthesis-Shader
SOURCE         = $(MAIN).cpp
OBJECT         = $(MAIN).o
COMPILE        = g++ -std=c++17 -Wall -Wextra -pedantic -c
LINK           = g++ -std=c++17 -Wall -Wextra -pedantic -o
FLAGS	       = -lglfw -lGL -lGLEW
REMOVE         = rm -f

all: $(MAIN)

$(MAIN): $(SOURCE)
	$(LINK) $(MAIN) $(SOURCE) $(FLAGS)

$(OBJECT): $(SOURCE)
	$(COMPILE) $(SOURCE) -o $(FLAGS)

test: test.o
	$(LINK) test test.o $(FLAGS)

test.o: test.cpp
	$(COMPILE) test.cpp -o test.o

clean:
	$(REMOVE) $(MAIN) $(OBJECT) test
