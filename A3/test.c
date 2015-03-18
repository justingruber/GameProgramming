#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "graphics.h"



void LoadGLTextures() {
    // Load Texture
    Image *image1;
    // allocate space for texture
    image1 = (Image *) malloc(sizeof(Image));
    if (image1 == NULL) {
    printf("Error allocating space for image");
    exit(0);
    }

    if (!ImageLoad("grass.bmp", image1)) {
    exit(1);
    }

    // Create Texture
    glGenTextures(1, &texture[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);   // 2d texture (x and y size)

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture

    // 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image, y size from image,
    // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
    glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image1->data);
}


int main()
{

glEnable(GL_DEPTH_TEST);
glDepthMask(GL_TRUE);
glDepthFunc(GL_LEQUAL);
glEnable(GL_TEXTURE_2D);
glShadeModel(GL_SMOOTH);
LoadGLTextures();
glBindTexture(GL_TEXTURE_2D, texture[0]);
while (App.IsOpened())
{
    App.SetActive();
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glBegin(GL_QUADS);
    //back
    glColor3f(0.0f,0.0f,0.0f); //cyan
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.2f,0.2f,0.2f); //top left
    glTexCoord2f(1.0f, 1.0f); glVertex3f(0.2f,0.2f,0.2f); //top right
    glTexCoord2f(1.0f, 0.0f); glVertex3f(0.2f,-0.2f,0.2f); //bottom right
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.2f,-0.2f,0.2f); //bottom left
    //bottom
    glColor3f(0.0f,1.0f,0.0f); //green
    glVertex3f(0.2f,-0.2f,0.2f); //back right
    glVertex3f(-0.2f,-0.2f,0.2f); //back left
    glVertex3f(-0.2f,-0.2f,-0.2f); //front left
    glVertex3f(0.2f,-0.2f,-0.2f); //front right
    //front
    glColor3f(0.0f,0.0f,1.0f); //blue
    glVertex3f(0.2f,-0.2f,-0.2f); //bottom right
    glVertex3f(-0.2f,-0.2f,-0.2f); //bottom left
    glVertex3f(-0.2f,0.2f,-0.2f); //top left
    glVertex3f(0.2f,0.2f,-0.2f); //top right
    //top
    glColor3f(1.0f,1.0f,0.0f); //yellow
    glVertex3f(0.2f,0.2f,-0.2f); //front right
    glVertex3f(-0.2f,0.2f,-0.2f); //front left
    glVertex3f(-0.2f,0.2f,0.2f); //back left
    glVertex3f(0.2f,0.2f,0.2f); //back right
    //left
    glColor3f(0.0f,1.0f,1.0f); //pink
    glVertex3f(-0.2f,-0.2f,-0.2f); //bottom front
    glVertex3f(-0.2f,-0.2f,0.2f); //bottom back
    glVertex3f(-0.2f,0.2f,0.2f); //top back
    glVertex3f(-0.2f,0.2f,-0.2f); //top front
    //right
    glColor3f(1.0f,0.0f,0.0f); //red
    glVertex3f(0.2f,-0.2f,-0.2f); //bottom front
    glVertex3f(0.2f,-0.2f,0.2f); //bottom back
    glVertex3f(0.2f,0.2f,0.2f); //top back
    glVertex3f(0.2f,0.2f,-0.2f); //top front
    glEnd();

    glFlush();

    
}
return EXIT_SUCCESS;
}