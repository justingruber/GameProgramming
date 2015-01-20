
/* Derived from scene.c in the The OpenGL Programming Guide */
/* Keyboard and mouse rotation taken from Swiftless Tutorials #23 Part 2 */
/* http://www.swiftless.com/tutorials/opengl/camera2.html */

/* Frames per second code taken from : */
/* http://www.lighthouse3d.com/opengl/glut/index.php?fps */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "graphics.h"

	/* initialize graphics library */
extern void graphicsInit(int *, char **);

	/* lighting control */
extern void setLightPosition(GLfloat, GLfloat, GLfloat);
extern GLfloat* getLightPosition();

	/* viewpoint control */
extern void setViewPosition(float, float, float);
extern void getViewPosition(float *, float *, float *);
extern void getOldViewPosition(float *, float *, float *);
extern void getViewOrientation(float *, float *, float *);

	/* add cube to display list so it will be drawn */
extern int addDisplayList(int, int, int);

	/* mob controls */
extern void createMob(int, float, float, float, float);
extern void setMobPosition(int, float, float, float, float);
extern void hideMob(int);
extern void showMob(int);

	/* player controls */
extern void createPlayer(int, float, float, float, float);
extern void setPlayerPosition(int, float, float, float, float);
extern void hidePlayer(int);
extern void showPlayer(int);

	/* flag which is set to 1 when flying behaviour is desired */
extern int flycontrol;
	/* flag used to indicate that the test world should be used */
extern int testWorld;
	/* list and count of polygons to be displayed, set during culling */
extern int displayList[MAX_DISPLAY_LIST][3];
extern int displayCount;
	/* flag to print out frames per second */
extern int fps;
	/* flag to indicate removal of cube the viewer is facing */
extern int dig;
	/* flag indicates the program is a client when set = 1 */
extern int netClient;
	/* flag indicates the program is a server when set = 1 */
extern int netServer; 

	/* frustum corner coordinates, used for visibility determination  */
extern float corners[4][3];

	/* determine which cubes are visible e.g. in view frustum */
extern void ExtractFrustum();
extern void tree(float, float, float, float, float, float, int);

/********* end of extern variable declarations **************/
// float perlinNoise();
// float *** noise();
// float interpolate(float *);


	/*** collisionResponse() ***/
	/* -performs collision detection and response */
	/*  sets new xyz  to position of the viewpoint after collision */
	/* -can also be used to implement gravity by updating y position of vp*/
	/* note that the world coordinates returned from getViewPosition()
	   will be the negative value of the array indices */
void collisionResponse() {
	/* your collision code goes here */
   float currx = 0, curry = 0, currz = 0;
   float prevx = 0, prevy = 0, prevz = 0;

   getViewPosition(&currx, &curry, &currz);
   
   #ifdef debug
   fprintf(stderr, "Current position in the world: x: %d, y: %d, z: %d\n", (int)abs(currx), (int)abs(curry), (int)abs(currz));
   fprintf(stderr, "Cube ID: %d\n", world[(int)abs(currx)][(int)abs(curry)][(int)abs(currz)]);
   #endif

   switch(world[(int)abs(currx)][(int)abs(curry)][(int)abs(currz)]){
      case 0:
         break;
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
         #ifdef debug
         fprintf(stderr, "Collision detected.\n");
         #endif
         getOldViewPosition(&prevx, &prevy, &prevz);
         setViewPosition(prevx, prevy, prevz);
         break;
   }
   return;

}


	/*** update() ***/
	/* background process, it is called when there are no other events */
	/* -used to control animations and perform calculations while the  */
	/*  system is running */
	/* -gravity must also implemented here, duplicate collisionResponse */
void update() {
int i, j, k;
float *la;

	/* sample animation for the test world, don't remove this code */
	/* -demo of animating mobs */
   if (testWorld) {
	/* sample of rotation and positioning of mob */
	/* coordinates for mob 0 */
      static float mob0x = 50.0, mob0y = 25.0, mob0z = 52.0;
      static float mob0ry = 0.0;
      static int increasingmob0 = 1;
	/* coordinates for mob 1 */
      static float mob1x = 50.0, mob1y = 25.0, mob1z = 52.0;
      static float mob1ry = 0.0;
      static int increasingmob1 = 1;

	/* move mob 0 and rotate */
	/* set mob 0 position */
      setMobPosition(0, mob0x, mob0y, mob0z, mob0ry);

	/* move mob 0 in the x axis */
      if (increasingmob0 == 1)
         mob0x += 0.2;
      else 
         mob0x -= 0.2;
      if (mob0x > 50) increasingmob0 = 0;
      if (mob0x < 30) increasingmob0 = 1;

	/* rotate mob 0 around the y axis */
      mob0ry += 1.0;
      if (mob0ry > 360.0) mob0ry -= 360.0;

	/* move mob 1 and rotate */
      setMobPosition(1, mob1x, mob1y, mob1z, mob1ry);

	/* move mob 1 in the z axis */
	/* when mob is moving away it is visible, when moving back it */
	/* is hidden */
      if (increasingmob1 == 1) {
         mob1z += 0.2;
         showMob(1);
      } else {
         mob1z -= 0.2;
         hideMob(1);
      }
      if (mob1z > 72) increasingmob1 = 0;
      if (mob1z < 52) increasingmob1 = 1;

	/* rotate mob 1 around the y axis */
      mob1ry += 1.0;
      if (mob1ry > 360.0) mob1ry -= 360.0;
    /* end testworld animation */
   } else {

	/* your code goes here */

   }
}


float noise(int x, int y){

   int n = x + y * 57;
   n = (n<<13) ^ n;
   return (1.0 - ((n * (n * n * 15731 + 789221) + 1376312589)) / 107374182.0);
}

float smoothedNoise(float x, float y){

   float corners = (noise(x-1, y-1) + noise(x+1, y-1) + noise(x-1, y+1) + noise(x+1, y+1)) / 16;
   float sides = (noise(x-1, y) + noise(x+1, y) + noise(x, y-1) + noise(x, y+1)) / 8;
   float center = noise(x, y) / 4;
   return corners + sides + center;
}

float interpolatedNoise(float x, float y){

   int wholeX = (int)x;
   float remainderX = x - wholeX;

   int wholeY = (int)y;
   float remainderY = y - wholeY;

   float vector1 = smoothedNoise(wholeX, wholeY);
   float vector2 = smoothedNoise(wholeX + 1, wholeY);
   float vector3 = smoothedNoise(wholeX, wholeY + 1);
   float vector4 = smoothedNoise(wholeX + 1, wholeY + 1);

   int interpolate1 = (vector1, vector2, remainderX);
   int interpolate2 = (vector3, vector4, remainderY);
}


int interpolate(int a, int b, float x){
   
   float ft = x * 3.1415927;
   float f = (1 - cos(ft)) * .5;
   return  a*(1-f) + b*f;
}

int perlinNoise(float x, float y){


   int i = 0;
   int total = 0;
   int persistence = 1;
   int octaves = 8;
   int divAmplititude[6] = {1, 4, 16, 64, 256, 1024};

   for(i = 0; i < octaves; i++){
      
   }

}


// float perlinNoise(){
//    float amplitude = 1.0;
//    float octaves = 3.0;
//    float frequency = 1.75;
//    float ***octaveArray[(int)octaves];
//    int i,j,k;
   

//    for(i = 0; i < 4; i++){
//       octaveArray[i] = noise();
//    }
//    //interpolate(*octaveArray);
   

// }


// float *** noise(){
//    srand(SEED);
   
//    static float octave[WORLDX][WORLDY][WORLDZ - 1];
//    int i,j,k;
//    for(i = 0; i < WORLDX; i++){
//       for (j = 0; j < WORLDY; j++){
//          for(k = 0; k < WORLDZ - 1; k++){
//             octave[i][j][k] = (float)rand() / (float)RAND_MAX * 2 - 1;
//             #ifdef debugMapGen
//             printf("%f ", octave[i][j][k]);
//             #endif
//          }
//          #ifdef debugMapGen
//          printf("\n");
//          #endif
//       }
//    } 
//    return octave;
// }


// float interpolate(float *octaveArray){
//    int i,j,k;

//    for(i = 0; i < WORLDX; i++){
//       for (j = 0; j < WORLDY; j++){
//          for(k = 0; k < WORLDZ - 1; k++){
//             // *octaveArray[i][j][k] = 0;
//          }
//       }
//    } 

//    return;
// }




int main(int argc, char** argv)
{
int i, j, k;
	/* initialize the graphics system */
   graphicsInit(&argc, argv);

	/* the first part of this if statement builds a sample */
	/* world which will be used for testing */
	/* DO NOT remove this code. */
	/* Put your code in the else statment below */
	/* The testworld is only guaranteed to work with a world of
		with dimensions of 100,50,100. */
   if (testWorld == 1) {
	/* initialize world to empty */
      for(i=0; i<WORLDX; i++)
         for(j=0; j<WORLDY; j++)
            for(k=0; k<WORLDZ; k++)
               world[i][j][k] = 0;
	
	/* some sample objects */
	/* build a red platform */
      for(i=0; i<WORLDX; i++) {
         for(j=0; j<WORLDZ; j++) {
            world[i][24][j] = 3;
         }
      }
	/* create some green and blue cubes */
      world[50][25][50] = 1;
      world[49][25][50] = 1;
      world[49][26][50] = 1;
      world[52][25][52] = 2;
      world[52][26][52] = 2;

	/* blue box shows xy bounds of the world */
      for(i=0; i<WORLDX-1; i++) {
         world[i][25][0] = 2;
         world[i][25][WORLDZ-1] = 2;
      }
      for(i=0; i<WORLDZ-1; i++) {
         world[0][25][i] = 2;
         world[WORLDX-1][25][i] = 2;
      }

	/* create two sample mobs */
	/* these are animated in the update() function */
      createMob(0, 50.0, 25.0, 52.0, 0.0);
      createMob(1, 50.0, 25.0, 52.0, 0.0);

	/* create sample player */
      createPlayer(0, 52.0, 27.0, 52.0, 0.0);

   } else {

	/* your code to build the world goes here */
      // perlinNoise();


   }


	/* starts the graphics processing loop */
	/* code after this will not run until the program exits */
   glutMainLoop();
   return 0; 
}

