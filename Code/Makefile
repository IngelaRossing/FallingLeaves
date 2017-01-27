# Project: GLprimer
# Makefile created by stegu 2014-01-16

# Makefile for Windows mingw32, Linux and MacOSX (gcc environments)

CC   = gcc
OBJ  = GLprimer.o pollRotator.o tgaloader.o tnm046.o triangleSoup.o
INC  = -I. -IC:/Dev-Cpp/include -I/usr/X11/include -I/usr/include
OPT = -Wall -O3 -ffast-math -g3
APPNAME = GLprimer

Usage:
	@echo "Usage: make Win32 | Linux | MacOSX | clean | distclean"

GLprimer.o: GLprimer.c
	$(CC) $(OPT) $(INC) -DAPPBUNDLE -c GLprimer.c -o GLprimer.o

pollRotator.o: pollRotator.c
	$(CC) $(OPT) $(INC) -c pollRotator.c -o pollRotator.o

tgaloader.o: tgaloader.c
	$(CC) $(OPT) $(INC) -c tgaloader.c -o tgaloader.o

tnm046.o: tnm046.c
	$(CC) $(OPT) $(INC) -c  tnm046.c -o tnm046.o

triangleSoup.o: triangleSoup.c
	$(CC) $(OPT) $(INC) -c  triangleSoup.c -o triangleSoup.o

Win32: $(OBJ)
	$(CC) $(OBJ) -o $(APPNAME).exe -L. -LC:/Dev-Cpp/lib -mwindows -lglfw3 -lopengl32 -mconsole -g3

Linux: $(OBJ)
	$(CC) $(OBJ) -lglfw3 -o $(APPNAME)

MacOSX: $(OBJ)
	bash bundle.sh $(APPNAME)
	$(CC) -L./GLFW $(OBJ) -o $(APPNAME).app/Contents/MacOS/$(APPNAME) -lglfw3_macosx -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo

clean:
	rm -f $(OBJ)

distclean:
	rm -rf $(OBJ) $(APPNAME) $(APPNAME).exe $(APPNAME).app
