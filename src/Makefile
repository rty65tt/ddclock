
VER=1.0

GCC=g++
RC=windres
EXE=ddclock-${VER}.exe
LDFLAGS=-s -O2 -static -mwindows -lgdiplus -lcomctl32 -static-libgcc -static-libstdc++
CXXFLAGS=-O2 -Wl,-rpath
OBJS=resource.res main.o

.PHONY:all clean resource.res

all:${EXE}

clean:
	rm -rf ${EXE}
	rm -rf ${OBJS}

%.o: %.c
	${GCC} ${CXXFLAGS} -c "$<" -o "$@"

resource.res: resource.rc app.manifest icon.ico resource.h
	${RC} -i "$<" -O coff -o "$@"

${EXE}: ${OBJS}
	${GCC} ${OBJS} -o "$@" ${LDFLAGS} 
