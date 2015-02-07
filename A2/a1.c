
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

	/* mouse function called by GLUT when a button is pressed or released */
void mouse(int, int, int, int);

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


extern float vpx, vpy, vpz;
extern float oldvpx, oldvpy, oldvpz;

/********* end of extern variable declarations **************/

typedef struct projectile {
   float ang;
   float vel;
   // float speed;
   int id;
   int x,y,z;
   float rx, ry, rz;
   int isActive;
   float currTime;

} Projectile;

void genClouds();
void moveClouds();
void moveProjectiles();
float findHeightAtTimeT(float, float, float, float, int);
float findDistancetAtTimeT(float, float, float, float, int);
int currentTime;
int previousTime;
int timeElapsed;
int hasFired = 0;
float velocity = 0.5;
float angle = 45.0;
float xUp = 0, xDown = 0, xDifference = 0;
float yUp = 0, yDown = 0, yDifference = 0;
Projectile * projArray[MOB_COUNT];


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
   float xaxis;
   float yaxis;
   float zaxis;
   getViewPosition(&currx, &curry, &currz);      

   #ifdef debugCollision
      fprintf(stderr, "Current position in the world: x: %d, y: %d, z: %d\n", (int)abs(currx), (int)abs(curry), (int)abs(currz));
      fprintf(stderr, "Cube ID: %d\n", world[(int)abs(currx)][(int)abs(curry)][(int)abs(currz)]);
      getViewOrientation(&xaxis, &yaxis, &zaxis);
      printf("xaxis: %d, yaxis: %d, zaxis: %f\n", (int)xaxis % 360, (int)yaxis % 360, zaxis);
   #endif


   switch(world[(int)abs(currx)][(int)abs(curry)][(int)abs(currz)]){
      case 0:

         if(abs(currx) <= 0.1 || abs(currx) >= WORLDX ||
            abs(currz) <= 0.1 || abs(currz) >= WORLDZ ||
            abs(curry) <= 0.1 || abs(curry) >= WORLDY ){
            setViewPosition(oldvpx, oldvpy, oldvpz);
         }
         break;
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
         #ifdef debugCollision
            printf("currx: %f, curry: %f, currz: %f\n", currx, curry, currz);
            fprintf(stderr, "Collision detected. ceil(y): %f\n\n\n", -1*(ceil(abs(curry) + 1)));
         #endif

         if(world[(int)abs(currx)][(int)abs(curry) + 1][(int)abs(currz) + 1] == 0){
               setViewPosition(currx, curry - 1, currz);
         }
         else{
            setViewPosition(oldvpx, oldvpy, oldvpz);
         }
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
      int timeElapsed = glutGet(GLUT_ELAPSED_TIME);
   /* your code goes here */
      // printf("time: %d\n", glutGet(GLUT_ELAPSED_TIME) % 1000);

      if(timeElapsed % 100 > 80)
         moveProjectiles();
      if(timeElapsed % 1000 > 950)
         genClouds();
      if(timeElapsed % 150 > 130)
         moveClouds();

   }
   if(flycontrol == 1 ){
      float x,y,z;
      getViewPosition(&x, &y, &z);
      if(world[abs((int)x)][abs((int)y)-1][abs((int)z)] == 0){
         setViewPosition(x, y + 0.25, z);
      }
   }
}

void moveClouds(){
   int x, z;
   //Pulling the clouds though the map (checking for y-2 because of artifacts)
   for(x = 0; x < WORLDX; x++){
      for(z = 0; z < WORLDZ; z++){
         if(world[x][WORLDY - 2][z] == 5){
            if(z != WORLDZ){
               world[x][WORLDY - 2][z - 1] = 5;
               world[x][WORLDY - 2][z] = 0;
            }
         }
           
      }
   }
   for(x = 0; x < WORLDX; x++){
        world[x][WORLDY - 2][0] = 0;
   };
}

void genClouds(){
   int z;
   int position = (rand() % WORLDX) + 1;
   int z_pos = (rand() % WORLDZ) + 1;

   if(position + 6 < WORLDX){
      if(z_pos + 6 < WORLDZ){
         for(z = z_pos; z < z_pos + 5; z++){
               world[z][WORLDY - 2][position - 1] = 5;
               world[z][WORLDY - 2][position + 1] = 5;
               world[z][WORLDY - 2][position] = 5;
         }
      }
   }
}

//Code from initializeTables and Perlin was taken and modified from
//http://www.angelcode.com/dev/perlin/perlin.html
void initializeTables(int SIZE, int * persistence, float * gradientX, float * gradientY){
   int i, j;
   

   // Initialize the permutation table
   for(i = 0; i < SIZE; i++)
     persistence[i] = i;

   for(i = 0; i < SIZE; i++)
   {
     j = rand() % SIZE;

     int nSwap = persistence[i];
     persistence[i]  = persistence[j];
     persistence[j]  = nSwap;
   }
      
   // Generate the gradient look-up tables
   for(i = 0; i < SIZE; i++)
   {
     gradientX[i] = (float)(rand())/(RAND_MAX/2) - 1.0f; 
     gradientY[i] = (float)(rand())/(RAND_MAX/2) - 1.0f;
   }  
}

float perlin(float x, float y, int * persistence, float * gradientX, float * gradientY, int SIZE){
   
   // Compute the integer positions of the four surrounding points
   int qx0 = (int)floorf(x);
   int qx1 = qx0 + 1;

   int qy0 = (int)floorf(y);
   int qy1 = qy0 + 1;

   // Permutate values to get indices to use with the gradient look-up tables
   int q00 = persistence[(qy0 + persistence[qx0 % SIZE]) % SIZE];
   int q01 = persistence[(qy0 + persistence[qx1 % SIZE]) % SIZE];

   int q10 = persistence[(qy1 + persistence[qx0 % SIZE]) % SIZE];
   int q11 = persistence[(qy1 + persistence[qx1 % SIZE]) % SIZE];

   // Computing vectors from the four points to the input point
   float tx0 = x - floorf(x);
   float tx1 = tx0 - 1;

   float ty0 = y - floorf(y);
   float ty1 = ty0 - 1;

   // Compute the dot-product between the vectors and the gradients
   float v00 = gradientX[q00]*tx0 + gradientY[q00]*ty0;
   float v01 = gradientX[q01]*tx1 + gradientY[q01]*ty0;

   float v10 = gradientX[q10]*tx0 + gradientY[q10]*ty1;
   float v11 = gradientX[q11]*tx1 + gradientY[q11]*ty1;

   // Do the bi-cubic interpolation to get the final value
   float wx = (3 - 2*tx0)*tx0*tx0;
   float v0 = v00 - wx*(v00 - v01);
   float v1 = v10 - wx*(v10 - v11);

   float wy = (3 - 2*ty0)*ty0*ty0;
   float v = v0 - wy*(v0 - v1);

   return v;
}

void genWorld(){
   //Seeding for the pseudo-random world
   srand(SEED);
   
   int i,j,k;
   int SIZE = 256;

   //Integer arrays that are dyanmically made based off of 'SIZE'
   int * persistence;
   float * gradientX;
   float * gradientY;


   int octaves = 4;
   int modifier = 15;
   float height = 0;


   //Checking to see if malloc was successful for the arrays
   if(   ((persistence = malloc(sizeof(int) * SIZE)) == NULL)
      || ((gradientX = malloc(sizeof(int) * SIZE)) == NULL)
      || ((gradientY = malloc(sizeof(int) * SIZE)) == NULL)){
      fprintf(stderr, "Error: Not enough memory available.\n");
      exit(1);
   }



   for(i=0; i<WORLDX; i++)
      for(j=0; j<WORLDY; j++)
         for(k=0; k<WORLDZ; k++)
            world[i][j][k] = 0;
         
   // world[23][WORLDY-1][23] = 8;
   //Initializing the persistence tables and the gradient tables
   initializeTables(SIZE, persistence, gradientX, gradientY);


   for(i=0; i<WORLDX; i++) {
      for(j=0; j<WORLDZ; j++) {
         height = perlin((float)(i) / modifier, (float)(j) / modifier, persistence, gradientX, gradientY, SIZE);
         height = (height * 25) + 25;

         //Filling the map so the terrain is not hollow
         for(k = 0; k < (int)height; k++){
            if((int)height > 30 && k > 30){
               world[i][k][j] = 5;
            } else if((int)height < 20 && k < 20){
               world[i][k][j] = 2;
            } else{
               world[i][k][j] = 1;
            }
         }
      }
   }
}

void initProjectiles(){
   int i;

   for(i = 0; i < MOB_COUNT; i++){

      Projectile * tmp = malloc(sizeof(Projectile));
      tmp->isActive = 0;
      tmp->id = -1;
      tmp->ang = 0;
      tmp->vel = 0;
      tmp->x = 0;
      tmp->y = 0;
      tmp->z = 0;
      tmp->currTime = 0.0;
      projArray[i] = tmp;
   }
}

float findHeightAtTimeT(float initHeight, float theta, float currVelocity, float currTime, int direction){

   float acceleration = -0.98;
   float vy;
   float currVelocity2 = currVelocity * 10;
   if(direction >= 0 && direction <= 45)        //1
      vy = sin(theta) * currVelocity2;

   else if(direction > 45 && direction <= 90)   //2
      vy = cos(theta) * currVelocity2;

   else if(direction > 90 && direction <= 135)  //3
      vy = cos(theta) * currVelocity2;

   else if(direction > 135 && direction <= 180) //4
      vy = sin(theta) * currVelocity2;

   else if(direction > 180 && direction <= 225) //5
      vy = -1 * (sin(theta) * currVelocity2);

   else if(direction > 225 && direction <= 270) //6
      vy = -1 * (cos(theta) * currVelocity2);

   else if(direction > 270 && direction <= 315) //7
      vy = -1 * (cos(theta) * currVelocity2);

   else if(direction > 315 && direction <= 360) //8
      vy = -1 * (sin(theta) * currVelocity2);

   return initHeight + (vy * currTime) + ((0.5 * acceleration) *  pow(currTime, 2));
}

float findDistancetAtTimeT(float initDistance, float theta, float currVelocity, float currTime, int direction){
   float acceleration = 0.0;
   float vx;
   float currVelocity2 = currVelocity * 10;
   if(direction >= 0 && direction <= 45)        //1
      vx = cos(theta) * currVelocity2;

   else if(direction > 45 && direction <= 90)   //2
      vx = sin(theta) * currVelocity2;

   else if(direction > 90 && direction <= 135)  //3
      vx = -1 * sin(theta) * currVelocity2;

   else if(direction > 135 && direction <= 180) //4
      vx = -1 * cos(theta) * currVelocity2;

   else if(direction > 180 && direction <= 225) //5
      vx = -1 * (cos(theta) * currVelocity2);

   else if(direction > 225 && direction <= 270) //6
      vx = -1 * (sin(theta) * currVelocity2);

   else if(direction > 270 && direction <= 315) //7
      vx = sin(theta) * currVelocity2;

   else if(direction > 315 && direction <= 360) //8
      vx = cos(theta) * currVelocity2;

   return initDistance + (vx * currTime);
}


void moveProjectiles(){
   int i = 0;
   float x,y,z;
   float direction;
   float vx, yx;
   float currHeight, currXDistance, currZDistance;
   do{
      Projectile * currProjectile = projArray[i];
      if(currProjectile->isActive == 1){
         if(currProjectile->x >= WORLDX || currProjectile->z >= WORLDZ){
            hideMob(currProjectile->id);
            hasFired = 0;
            currProjectile->isActive = 0;
            currProjectile->currTime = 0.0;
         }
         else{
            int direction = (int)currProjectile->ry % 360;


            currHeight = findHeightAtTimeT(currProjectile->y, currProjectile->ang, currProjectile->vel, currProjectile->currTime, direction);
            currXDistance = findDistancetAtTimeT(currProjectile->x, currProjectile->ang, currProjectile->vel, currProjectile->currTime, direction);
            currZDistance = findDistancetAtTimeT(currProjectile->z, currProjectile->ang, currProjectile->vel, currProjectile->currTime, direction);

            printf("x: %d, y: %d, z: %d, currHeight: %f, angle: %f, velocity: %f, time: %f, direction: %d\n", currProjectile->x, currProjectile->y, currProjectile->z, currHeight, currProjectile->ang, currProjectile->vel, currProjectile->currTime, direction);
            
            if(world[currProjectile->x][(int)currHeight][currProjectile->z] == 0){
               setMobPosition(currProjectile->id, currXDistance, currHeight, currZDistance, 0.0);
               currProjectile->x += 1.0;
            }
            else{
               printf("boom!\n");
               hideMob(currProjectile->id);
               hasFired = 0;
               currProjectile->isActive = 0;
               currProjectile->currTime = 0.0;
            }
         }
         currProjectile->currTime += 0.5;
      }
      i++;
   }while(i < MOB_COUNT);
}

void shoot(){
   hasFired = 1;
   float x,y,z;
   float roll,yaw,pitch;
   float i = 0;

   getViewPosition(&x, &y, &z);
   getViewOrientation(&roll, &yaw, &pitch);
   
   // printf("x: %f, y: %f, z: %f\n",x, y, z);
   // printf("roll: %f, yaw: %f, pitch: %f\n", roll, yaw, pitch);
   
   do{
      Projectile * tmp = projArray[(int)i];
      if(tmp->isActive == 0){
         tmp->id = i;
         tmp->ang = angle;
         tmp->vel = velocity;
         tmp->x = abs(x);
         tmp->y = abs(y);
         tmp->z = abs(z);
         tmp->rx = roll;
         tmp->ry = yaw;
         tmp->rz = pitch;
         tmp->isActive = 1;
         createMob(tmp->id, abs(x), abs(y), abs(z), roll);
         break;
      }
      i++;
   }while(i < MOB_COUNT);
}

	/* called by GLUT when a mouse button is pressed or released */
	/* -button indicates which button was pressed or released */
	/* -state indicates a button down or button up event */
	/* -x,y are the screen coordinates when the mouse is pressed or */
	/*  released */ 
void mouse(int button, int state, int x, int y) {
   

   if (button == GLUT_LEFT_BUTTON){
      // printf("left button - ");
      if(state == GLUT_DOWN)
         shoot();
   }

   else if (button == GLUT_MIDDLE_BUTTON){
      // printf("middle button - ");
   }
   else{
      // printf("right button - ");
      if (state == GLUT_UP){
         // printf("up - ");
         xUp = (float)x;
         yUp = (float)y;
         xDifference = (xUp - xDown);
         yDifference = (yUp - yDown);
         // printf("xUp: %f, xDown: %f, yUp: %f, yDown: %f \nxDifference: %f, yDifference: %f \n", xUp, xDown, yUp, yDown, xDifference, yDifference);

         if(xDifference > 0){
            if(angle + xDifference < 90.0){
               angle += xDifference / 100.0;
               printf("Angle: %f\n", angle);
            }
         }
         else{
            if(angle + xDifference > 0.0){
               angle += xDifference / 100.0;
               printf("Angle: %f\n", angle);   
            }
         }

         if(yDifference > 0){
            if(velocity + yDifference < 100.0){
               velocity += yDifference / 100.0;
               printf("Velocity: %f\n", velocity);
            }
         }
         else{
            if(velocity + yDifference > 0.0){
               velocity += yDifference / 100.0;
               printf("Velocity: %f\n", velocity);
            }
         }

         // printf("xDifference: %f, yDifference: %f\n", xDifference, yDifference);
      }
      else{
         xDown = (float)x;
         yDown = (float)y;
      }
   }

   // if (state == GLUT_UP){
   //    printf("up - ");
   //    xUp = x;
   //    yUp = y;
   // }
   // else{
   //    printf("down - ");
   //    xDown = x;
   //    yDown = y;
   // }

   // printf("%d %d\n", x, y);
}



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
      genWorld();      
      genClouds();
      initProjectiles();
      currentTime = glutGet(GLUT_ELAPSED_TIME);
   }


	/* starts the graphics processing loop */
	/* code after this will not run until the program exits */
   glutMainLoop();
   return 0; 
}

