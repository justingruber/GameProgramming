//For Mac
// #include <OpenGL/gl.h>
// #include <OpenGL/glu.h>
// #include <GLUT/glut.h>

//For Linux
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

/* world size and storage array */
#define WORLDX 100
#define WORLDY 50
#define WORLDZ 100
GLubyte  world[WORLDX][WORLDY][WORLDZ];

#define MAX_DISPLAY_LIST 500000

#define MAX_DISPLAY_LIST 500000

#define MOB_COUNT 10
#define PLAYER_COUNT 10

// Uncomment this to enable debug information
// #define debugMapGen
// #define debugCollision

#define SEED 1131992
#define FPS 1000/10
#define PI 3.141592653
#define toRad(x) (x * (PI/180))
#define toDeg(x) (x * (180 / PI)) 