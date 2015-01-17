
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

        /* world size and storage array */
#define WORLDX 100
#define WORLDY 50
#define WORLDZ 100
GLubyte  world[WORLDX][WORLDY][WORLDZ];

#define MAX_DISPLAY_LIST 500000

//Uncomment this to enable debug information
#define debug

#define SEED 2949873429