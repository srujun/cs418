//
// Example code for OpenGL programming
//
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <OPENGL/gl3.h>


GLuint v,f,p;
GLint loc;

void reshape(int w, int h) {
    
    if(h == 0)
        h = 1;
    
    float ratio = 1.0* w / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(45,ratio,1,1000);
    glMatrixMode(GL_MODELVIEW);
    
    
}
float a = 0;

void display(void) {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glLoadIdentity();
    gluLookAt(0.0,0.0,10.0,
              0.0,0.0,-1.0,
              0.0f,1.0f,0.0f);
    
    
    glRotatef(a,0,1,0);
    //you send loc as your attribute "height" to vertex shader. Vertex specified in glVertex calls is available to access in vertex shader as gl_Vertex
    glBegin(GL_TRIANGLE_STRIP);
    glVertexAttrib1f(loc,2.0);
    glVertex2f(-1,1);
    
    glVertexAttrib1f(loc,2.0);
    glVertex2f(1,1);
    
    glVertexAttrib1f(loc,-2.0);
    glVertex2f(-1,-1);
    
    glVertexAttrib1f(loc,-2.0);
    glVertex2f(1,-1);
    
    glEnd();
    
    
    a+=0.1;
    
    glutSwapBuffers();
}

//you can program your own vertex shader and fragment shader
void setShaders() {
    
    char *vs = NULL,*fs = NULL;
    //Create vertex shader and fragment shader
    v = glCreateShader(GL_VERTEX_SHADER);
    f = glCreateShader(GL_FRAGMENT_SHADER);
    //define vertex and fragment shader source
    vs = "attribute float height;void main(){vec4 p;p.xz = gl_Vertex.xy;p.y = height;p.w = 1.0;gl_Position = gl_ModelViewProjectionMatrix * p;}";
    fs = "void main(){gl_FragColor = vec4(0.2,0.8,0.4,1.0);}";
    
    const char * vv = vs;
    const char * ff = fs;
    
    glShaderSource(v, 1, &vv,NULL);
    glShaderSource(f, 1, &ff,NULL);
    
    glCompileShader(v);
    glCompileShader(f);
   
    p = glCreateProgram();
    glAttachShader(p,v);
    glAttachShader(p,f);
    
    glLinkProgram(p);
    
    glUseProgram(p);
    
    
    loc = glGetAttribLocation(p,"height");
    
}


int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(320,320);
    glutCreateWindow("Hello Shader");
    
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(reshape);
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0,1.0,1.0,1.0);
    
    setShaders();
    
    glutMainLoop();
    
    return 0;
}

