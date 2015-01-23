
# INCLUDES = -F/System/Library/Frameworks -framework OpenGL -framework GLUT

INCLUDES = -I/usr/include/GL/ -lGL -lGLU -lglut -lm -g3

a1: a1.c graphics.c visible.c graphics.h
	gcc a1.c graphics.c visible.c $(INCLUDES) -o a1  

clean:
	rm a1