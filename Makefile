
BIN = bin
SRC = src
OBJ = obj
LIB = lib

NAME = minimum

TARGETS = minimum


# Compiler flags
CFLAGS = -g
CXXFLAGS = -g

# Linker flags
LDFLAGS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lGLEW -lglfw3

# Compiler being used
CC = gcc


#HEADERS = $(addprefix $(INC)/, something1.h, something2.h, something3.h )
OBJECTS= $(addprefix $(OBJ)/, minimum.o)


default : $(TARGETS)

$(TARGETS) : $(OBJECTS)
	$(CC) -o $@ $(CXXFLAGS) $(OBJECTS) $(LDFLAGS)

$(OBJ)/%.o: $(SRC)/%.c $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ -c $<


clean:
	-rm -f $(OBJ)/* $(BIN)/*
	-rm minimum
#	-rm ./*.zip

redo:
	touch *.c
	make

superclean:
	rm -f *.o
	rm *~
	rm -f $(TARGETS)

tgz:
	rm -f *.o
	rm *~
	rm -f $(TARGETS)
	tar -zcvf $(TARGETS).tgz *

zip:
	zip -l $(NAME).zip  ./* src/*.c include/*.h ./$(NAME).mak

# Generate hierarchy graph
dot:
	dot -Tpng classes.dot -o classes.png
