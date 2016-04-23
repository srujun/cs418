#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <ctime>
#include "GL/gl.h"
#include <GL/freeglut.h>

PFNGLWINDOWPOS2IPROC glWindowPos2i;

float sealevel;
float polysize;

bool shouldRoll = false;
bool shouldPitch = false;
bool shouldYaw = false;
float rollAngle = 0.0;
float pitchAngle = 0.0;
float yawAngle = 0.0;

float translateSpeed = 0.0009;

bool toggleColor = true;

time_t starttime;

int seed(float x, float y) {
	// constant a and b are adjusted by current time, which makes them randomized
    static int a = 1588635695 - time(NULL);
    static int b = 1117695901 - time(NULL);
	int xi = *(int *)&x;
	int yi = *(int *)&y;
    return ((xi * a) % b) - ((yi * b) % a);
}

void mountain(float x0, float y0, float z0, float x1,
	float y1, float z1, float x2, float y2, float z2, float s)
{
	float x01,y01,z01,x12,y12,z12,x20,y20,z20;

	if (s < polysize) {
		x01 = x1 - x0;
		y01 = y1 - y0;
		z01 = z1 - z0;

		x12 = x2 - x1;
		y12 = y2 - y1;
		z12 = z2 - z1;

		x20 = x0 - x2;
		y20 = y0 - y2;
		z20 = z0 - z2;

		float nx = y01*(-z20) - (-y20)*z01;
		float ny = z01*(-x20) - (-z20)*x01;
		float nz = x01*(-y20) - (-x20)*y01;

		float den = sqrt(nx*nx + ny*ny + nz*nz);

		if (den > 0.0) {
			nx /= den;
			ny /= den;
			nz /= den;
		}

		glNormal3f(nx,ny,nz);
		glBegin(GL_TRIANGLES);
			glVertex3f(x0,y0,z0);
			glVertex3f(x1,y1,z1);
			glVertex3f(x2,y2,z2);
		glEnd();

		return;
	}

	x01 = 0.5*(x0 + x1);
	y01 = 0.5*(y0 + y1);
	z01 = 0.5*(z0 + z1);

	x12 = 0.5*(x1 + x2);
	y12 = 0.5*(y1 + y2);
	z12 = 0.5*(z1 + z2);

	x20 = 0.5*(x2 + x0);
	y20 = 0.5*(y2 + y0);
	z20 = 0.5*(z2 + z0);

	s *= 0.5;

	srand(seed(x01,y01));
	z01 += 0.3*s*(2.0*((float)rand()/(float)RAND_MAX) - 1.0);
	srand(seed(x12,y12));
	z12 += 0.3*s*(2.0*((float)rand()/(float)RAND_MAX) - 1.0);
	srand(seed(x20,y20));
	z20 += 0.3*s*(2.0*((float)rand()/(float)RAND_MAX) - 1.0);

	mountain(x0,y0,z0,x01,y01,z01,x20,y20,z20,s);
	mountain(x1,y1,z1,x12,y12,z12,x01,y01,z01,s);
	mountain(x2,y2,z2,x20,y20,z20,x12,y12,z12,s);
	mountain(x01,y01,z01,x12,y12,z12,x20,y20,z20,s);
}

void init(void) 
{
	GLfloat white[] = {1.0,1.0,1.0,1.0};
	GLfloat lpos[] = {0.0,1.0,0.0,0.0};

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);

	gluLookAt(0.0, 0.0, 0.25, 0.5, 0.5, 0.25, 0.0, 0.0, 1.0);

	glClearColor (0.5, 0.5, 1.0, 0.0);
	/* glShadeModel (GL_FLAT); */
	glEnable(GL_DEPTH_TEST);

	sealevel = 0.0;
	polysize = 0.01;

	// set program start time
	starttime = time(NULL);
}

void display(void)
{
	static GLfloat angle = 0.0;

	GLfloat tanamb[] = {0.2,0.15,0.1,1.0};
	GLfloat tandiff[] = {0.4,0.3,0.2,1.0};

	GLfloat seaamb[] = {0.0,0.0,0.2,1.0};
	GLfloat seadiff[] = {0.0,0.0,0.8,1.0};
	GLfloat seaspec[] = {0.5,0.5,1.0,1.0};

	// set toggle setting to switch terrain color
	if(toggleColor)
	{
		tanamb[0] = 0.0;
		tanamb[1] = 0.1;
		tanamb[2] = 0.0;

		tandiff[0] = 0.05;
		tandiff[1] = 0.3;
		tandiff[2] = 0.1;
	}
	else
	{
		tanamb[0] = 0.2;
		tanamb[1] = 0.15;
		tanamb[2] = 0.1;

		tandiff[0] = 0.4;
		tandiff[1] = 0.3;
		tandiff[2] = 0.2;
	}

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f (1.0, 1.0, 1.0);

	// create the model-view matrix
	GLfloat mv[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mv);

	glLoadIdentity ();             /* clear the matrix */

	glTranslatef(0.0, 0.0, translateSpeed);

	// adjust roll, pitch and yaw based on toggle settings
	if(shouldRoll)
		glRotatef(rollAngle, 0.0, 0.0, 1.0);
	if(shouldPitch)
		glRotatef(pitchAngle, 1.0, 0.0, 0.0);
	if(shouldYaw)
		glRotatef(yawAngle, 0.0, 1.0, 0.0);

	glMultMatrixf(mv);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, tanamb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, tandiff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, tandiff);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0);

	mountain(0.0,0.0,0.0, 1.0,0.0,0.0, 0.0,1.0,0.0, 1.0);
	mountain(1.0,1.0,0.0, 0.0,1.0,0.0, 1.0,0.0,0.0, 1.0);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, seaamb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, seadiff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, seaspec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0);

	glNormal3f(0.0,0.0,1.0);
	glBegin(GL_QUADS);
		glVertex3f(0.0,0.0,sealevel);
		glVertex3f(1.0,0.0,sealevel);
		glVertex3f(1.0,1.0,sealevel);
		glVertex3f(0.0,1.0,sealevel);
	glEnd();

	// render text to the screen using GLUT function
	glColor3f(1.0, 1.0, 1.0); 
  	glWindowPos2i(10, 775);
  	char speedmsg[20];
  	sprintf(speedmsg, "Current Speed: %d", (int)(translateSpeed*10000));
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)speedmsg );

	glWindowPos2i(10, 750);
  	char timemsg[30];
  	int elapsed = (int)(time(NULL) - starttime);
  	sprintf(timemsg, "Time Elapsed: %02d:%02d", elapsed / 60, elapsed % 60);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)timemsg );

	glutSwapBuffers();
	glFlush ();

	angle += 1.0;

	glutPostRedisplay();
}

void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(90.0,1.0,0.01,10.0);
	glMatrixMode (GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
	// key bindings
	if(key == 'w')
	{
		shouldPitch = true;
		pitchAngle = 0.75;
	}
	if(key == 'a')
	{
		shouldRoll = true;
		rollAngle = -1.5;
	}
	if(key == 's')
	{
		shouldPitch = true;
		pitchAngle = -0.75;
	}
	if(key == 'd')
	{
		shouldRoll = true;
		rollAngle = 1.5;
	}
	if(key == 'q')
	{
		shouldYaw = true;
		yawAngle = -0.75;
	}
	if(key == 'e')
	{
		shouldYaw = true;
		yawAngle = 0.75;
	}
	if(key == 'z')
		translateSpeed += 0.0001;
	if(key == 'x')
		translateSpeed -= 0.0001;

	switch (key) {
		case 't':
			toggleColor = !toggleColor;
			break;
		case '-':
			sealevel -= 0.01;
			break;
		case '+':
		case '=':
			sealevel += 0.01;
			break;
		case 'f':
			polysize *= 0.5;
			break;
		case 'c':
			polysize *= 2.0;
			break;
		case 27:
			exit(0);
			break;
	}
}

void keyboardUp(unsigned char key, int x, int y)
{
	// toggle settings for when key press is released
	if(key == 'w')
	{
		shouldPitch = false;
		pitchAngle = 0.0;
	}
	if(key == 'a')
	{
		shouldRoll = false;
		rollAngle = 0.0;
	}
	if(key == 's')
	{
		shouldPitch = false;
		pitchAngle = 0.0;
	}
	if(key == 'd')
	{
		shouldRoll = false;
		rollAngle = 0.0;
	}
	if(key == 'q')
	{
		shouldYaw = false;
		yawAngle = 0.0;
	}
	if(key == 'e')
	{
		shouldYaw = false;
		yawAngle = 0.0;
	}
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	// create windowpos variable
	glWindowPos2i = (PFNGLWINDOWPOS2IPROC) glutGetProcAddress("glWindowPos2i");
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (800, 800); 
	glutInitWindowPosition (380, 100);
	glutCreateWindow (argv[0]);
	init ();
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutMainLoop();
	return 0;
}
