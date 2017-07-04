# Finalproject
EXE=finalproject

# Main target
all: $(EXE)

#  MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall 
LIBS=-lglut32cu -lglu32 -lopengl32
CLEAN=del *.exe *.o *.a
else
#  OSX
ifeq "$(shell uname)" "Darwin"
CFLG=-O3 -Wall -Wno-deprecated-declarations
LIBS=-framework GLUT -framework OpenGL
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall
#LIBS=-lglut -lGLU -lGL -lm `sdl2-config --cflags --libs` -lSDL2main -lSDL2
LIBS=-lglut -lGLU -lGL -lm
endif
#  OSX/Linux/Unix/Solaris
CLEAN=rm -f $(EXE) *.o *.a
endif

# Dependencies
ball.o:			ball.c fishtank.h
bubble.o:		bubble.c fishtank.h
cylinder.o: 	cylinder.c fishtank.h
solidfish.o:    solidfish.c fishtank.h
final.o: 		final.c fishtank.h
plants.o: 		plants.c fishtank.h
sub.o: 			sub.c fishtank.h
tank.o:			tank.c fishtank.h
texload.o:		texload.c fishtank.h
loadcaustic.o:	loadcaustic.cpp vec3f.h fishtank.h
main.o:	    	main.cpp fishtank.h vec3f.h
vec3f.o:		vec3f.cpp vec3f.h
errcheck.o: 	errcheck.c CSCIx229.h
fatal.o: 		fatal.c CSCIx229.h
loadtexbmp.o: 	loadtexbmp.c CSCIx229.h


#  Create archive
CSCIx229.a:fatal.o loadtexbmp.o errcheck.o
	ar -rcs $@ $^

# Compile rules
.cpp.o:
	g++ -c $(CFLG) $<
.c.o:
	gcc -c $(CFLG) $<


#  Link
finalproject: main.o CSCIx229.a ball.o bubble.o cylinder.o solidfish.o plants.o sub.o tank.o texload.o loadcaustic.o vec3f.o
	g++ -O3 -o $@ $^   $(LIBS)

#  Clean
clean:
	$(CLEAN)
