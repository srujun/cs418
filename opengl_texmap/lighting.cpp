//
// Example code for Lighting
//
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <ctime>

int nFPS = 20;
float fRotateAngle = 0.f;

GLfloat mat_ambient[] = { 1, 0, 0, 1.0 };
GLfloat mat_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_shininess[] = { 50.0 };

GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };


void init(void)
{
    // init your data, setup OpenGL environment here
    glClearColor(0.9,0.9,0.9,1.0); // clear color is gray
    glPointSize(4.0);
    //glShadeModel(GL_SMOOTH);
    glMaterialfv(GL_FRONT,GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT,GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable( GL_LIGHT0);
    glEnable( GL_LIGHTING );
    glEnable(GL_DEPTH_TEST);

}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); // reset transformation matrix to identity
    gluLookAt(0.f,0.f,3.f,0.f,0.f,0.f,0.f,1.f,0.f);
    glRotatef(fRotateAngle,0.f,1.f,0.f);
    
    // Test drawing a solid teapot
    glColor3f(1.0,0.0,0.0); // set current color to Red
    glutSolidSphere (1.0, 20, 16);
    glutSwapBuffers();  // swap front/back framebuffer to avoid flickering
    
}

void reshape (int w, int h)
{
    // reset viewport ( drawing screen ) size
    glViewport(0, 0, w, h);
    float fAspect = ((float)w)/h;
    // reset OpenGL projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.f,fAspect,0.001f,30.f);
}


void timer(int v)
{
    fRotateAngle += 1.f; // change rotation angles
    glutPostRedisplay(); // trigger display function by sending redraw into message queue
    glutTimerFunc(1000/nFPS,timer,v); // restart timer again
}

int main(int argc, char* argv[])
{
    printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
    glutInit(&argc, (char**)argv);
    // set up for double-buffering & RGB color buffer & depth test
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize (500, 500);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ((const char*)"Hello Light");
    init(); // setting up user data & OpenGL environment
    // set up the call-back functions
    glutDisplayFunc(display);  // called when drawing
    glutReshapeFunc(reshape);  // called when change window size
    glutTimerFunc(100,timer,nFPS); // a periodic timer. Usually used for updating animation
    glutMainLoop(); // start the main message-callback loop
    
    return 0;
}