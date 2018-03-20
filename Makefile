
VER=1.3

GCC			= g++
RC			= windres
EXE			= ddclock.$(VER).exe
LDFLAGS		= -s -static -mwindows -lgdiplus -static-libstdc++ 
CXXFLAGS	= -Os -Wall
OBJS		= $(OBJ)/resource.res $(OBJ)/ddclock.o
BIN			= release
SRC			= src
OBJ			= obj

.PHONY:all clean 

all:$(BIN)/$(EXE)

clean:
	rm -rf $(BIN)/$(EXE)
	rm -rf $(OBJS)

$(OBJ)/%.o: $(SRC)/%.cpp
	$(GCC) $(CXXFLAGS) -c  "$<" -o "$@"  

$(OBJ)/resource.res: $(SRC)/resource.rc
	$(RC) -i "$<" -O coff -o "$@"

$(BIN)/$(EXE): $(OBJS)
	$(GCC) $(OBJS)  -o "$@" $(LDFLAGS)


