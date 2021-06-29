#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include <time.h> 
#include <stdlib.h>
#include <unistd.h>
using namespace std;
/*
void nextGenerationSquare(int x1, int y1,float* rgb)
{
    glColor3f(rgb[0],rgb[1],rgb[2]);
    glBegin(GL_QUADS);
	float xd = 1.0 / (LENGTH+BUFFER);
	float yd = 1.0 / (HEIGHT+BUFFER);
	float xf=x1- (LENGTH+BUFFER/2.0)/2.0;
	float yf=y1- (HEIGHT+BUFFER/2.0)/2.0;
	xf = xf/(LENGTH/2.0 + BUFFER/2.0) +xd*(BUFFER)/2.0;
	yf = yf/(HEIGHT/2.0 + BUFFER/2.0) +yd*(BUFFER)/2.0;
	xf += xd;
	yf += yd;
	xd=xd*SCALE;
	yd=yd*SCALE;
    glVertex2d(xf + xd, yf + yd);
    glVertex2d(xf + xd, yf - yd);
    glVertex2d(xf - xd, yf - yd);
    glVertex2d(xf - xd, yf + yd);

    glEnd();
}
*/
#define VERTS 15

void RenderCircle(float x, float y, float r){
	glColor3f(0,1,0);
	glBegin(GL_POLYGON);
	for (int i =0;i<VERTS;i++){
		glVertex2d(x + r*sin(2*i*M_PI/VERTS),y+ r*cos(2*i*M_PI/VERTS));
	}
	glEnd();
}

void disInit(){
	glClearColor(0.1f, 0.1f, 0.1f, 0); // Set background color to black and opaque
	glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer
	RenderCircle(0,0,0.05);
	glFlush();
}

void run(){
}


int main(int argc, char** argv) {
	srand (time(NULL));
	glutInit(&argc, argv);		// Initialize GLUT
    glutInitWindowSize(800,800);   // Set the window's initial width & height
    glutCreateWindow("Lines"); // Create a window with the given title
    glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
    glutDisplayFunc(disInit); // Register display callback handler for window re-paint
	glutIdleFunc(run);
	glutMainLoop();
	return 0;
}
