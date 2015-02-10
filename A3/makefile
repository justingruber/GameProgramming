# Flags for Mac
# INCLUDES = -F/System/Library/Frameworks -framework OpenGL -framework GLUT -lm

# Flags for Linux
INCLUDES = -I/usr/include/GL/ -lGL -lGLU -lglut -lm -g3

a1: a1.c graphics.c visible.c graphics.h
	gcc a1.c graphics.c visible.c -o a1 $(INCLUDES) 

clean:
	rm a1