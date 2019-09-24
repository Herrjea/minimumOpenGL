
BIN = bin
INC = include
LIB = lib
OBJ = obj
SRC = src

NAME = bin/minimum

TARGETS = bin/minimum


# Compiler flags
CFLAGS = -g -I$(INC)
CXXFLAGS = -g -I$(INC)

# Linker flags
LDFLAGS = -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lGLEW -lglfw

# Compiler being used
CC = gcc


HEADERS = $(addprefix $(INC)/, stb_image.h cglm/cglm.h )
OBJECTS= $(addprefix $(OBJ)/, minimum.o)


default : $(TARGETS)

$(TARGETS) : $(OBJECTS)
	$(CC) -o $@ $(CXXFLAGS) $(OBJECTS) $(LDFLAGS)

$(OBJ)/%.o: $(SRC)/%.c $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ -c $<


clean:
	-rm -f $(OBJ)/* $(BIN)/*
#	-rm ./*.zip

redo:
	touch *.c
	make

superclean:
	rm -f *.o
	rm -f $(TARGETS)

tgz:
	rm -f *.o
	rm -f $(TARGETS)
	tar -zcvf $(TARGETS).tgz *

zip:
	zip -l $(NAME).zip  ./* src/*.c include/*.h ./$(NAME).mak

# Generate hierarchy graph
dot:
	dot -Tpng classes.dot -o classes.png
