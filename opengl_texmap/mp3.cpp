#include <cstdlib>
#include <iostream>
#include <string>
#include <GL/gl.h>
#include <GL/freeglut.h>

#include "mp3.h"

Teapot * teapot;
Image * texture_map;
Image * env_map;

GLuint texmapID;
GLuint envmapID;

// Keyboard Toggles
enum DisplayMode
{
    solid, texture, environment
};

bool displayMesh = false;
bool lightsOn = true;
DisplayMode mode = solid;

// View Variables
float rotAngle = 0.0f;
int nFPS = 30;

// Materials
GLfloat mat_ambient[] = { 1.0f, 0.3f, 0.0f, 1.0f };
GLfloat mat_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
GLfloat mat_specular[] = { 0.6f, 0.6f, 0.6f, 1.0 };
GLfloat mat_shininess[] = { 20.0 };

GLfloat mat_ambient_tex[] = { 0.8f, 0.8f, 1.0f, 1.0f };
GLfloat mat_diffuse_tex[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat mat_specular_tex[] = { 0.8f, 0.8f, 1.0f, 1.0f };
GLfloat mat_shininess_tex[] = { 5.0 };

// LIGHTS!!! :)
GLfloat light_ambient[] = { 0.3, 0.3, 0.3, 1.0 };
GLfloat light_diffuse[] = { 0.7, 0.5, 0.1, 1.0 };
GLfloat light_specular[] = { 0.2, 0.2, 0.2, 1.0 };
GLfloat light_position[] = { -3.0, 3.0, 6.0, 0.0 };

void loadTextureMap()
{
    // generate and bind enviroment map
    glBindTexture(GL_TEXTURE_2D, texmapID);

    // Set the texture parameters
    glEnable(GL_TEXTURE_2D);
    // linear interpolation of pixel data
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // modulate, so we can merge the lights with the model texture
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    // set other params
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // set the environment texture's image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 texture_map->getWidth(), texture_map->getHeight(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_map->getBytes());
}

void loadEnviromentMap()
{
    // generate and bind enviroment map
    glBindTexture(GL_TEXTURE_2D, envmapID);

    // set environment map parameters
    glEnable(GL_TEXTURE_2D);
    // linear interpolation of pixel data
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // set texture generation mode to sphere map
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    // set other params
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // set the environment texture's image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 env_map->getWidth(), env_map->getHeight(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, env_map->getBytes());
}

void init(string objfile, string texmapfile, string envmapfile)
{
    // init your data, setup OpenGL environment here
    glClearColor(0.2, 0.2, 0.2, 1.0); // clear color is gray
    glPointSize(4.0);

    // assign the teapot model's material
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    // assign the light's ambient, diffuse, specular and position values
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    // enable lighting
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    // enable hidden surface calculation using z-buffers
    glEnable(GL_DEPTH_TEST);

    cout << "Creating Teapot in memory..." << endl;
    teapot = new Teapot(objfile);
    cout << "Teapot memory allocated." << endl;

    texture_map = new Image(texmapfile);
    env_map = new Image(envmapfile);
    // generate texture names
    glGenTextures(1, &texmapID);
    glGenTextures(1, &envmapID);
    cout << "Texture and Environment maps loaded." << endl;
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set whether mesh is displayed or not
    if(displayMesh)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(3.0f, 5.0f, 3.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    glRotatef(rotAngle, 0.f, 1.f, 0.f);

    // Draw the Teapot Model
    // We will loop through each face and draw the triangle
    // formed by that face's 3 vertices
    for(unsigned int i = 0; i < teapot->faces.size(); i++)
    {
        glBegin(GL_TRIANGLES);

        // get the current face
        Face f = teapot->faces[i];

        // get this face's 3 vertices, and the respective vertex-normals
        // also convert the vertices to cylindrical coordinates
        Vertex vert0   = teapot->vertices[f.vidx0];
        Vertex vert0cy = vert0.getCylindrical(teapot->maxYval);
        Point3D norm0  = teapot->vertexNormals[f.vidx0];

        Vertex vert1   = teapot->vertices[f.vidx1];
        Vertex vert1cy = vert1.getCylindrical(teapot->maxYval);
        Point3D norm1  = teapot->vertexNormals[f.vidx1];

        Vertex vert2   = teapot->vertices[f.vidx2];
        Vertex vert2cy = vert2.getCylindrical(teapot->maxYval);
        Point3D norm2  = teapot->vertexNormals[f.vidx2];

        if(mode == texture)
        {
            glTexCoord2f(vert0cy.x, vert0cy.y);
            glTexCoord2f(vert1cy.x, vert1cy.y);
            glTexCoord2f(vert2cy.x, vert2cy.y);
        }

        // specify the the normal first, then the vertex
        glNormal3f(norm0.x, norm0.y, norm0.z);
        glVertex3f(vert0.x, vert0.y, vert0.z);

        glNormal3f(norm1.x, norm1.y, norm1.z);
        glVertex3f(vert1.x, vert1.y, vert1.z);

        glNormal3f(norm2.x, norm2.y, norm2.z);
        glVertex3f(vert2.x, vert2.y, vert2.z);

        glEnd();
    }

    // enable/disable the lights as specified
    if(lightsOn)
        glEnable(GL_LIGHT0);
    else
        glDisable(GL_LIGHT0);

    if(mode == solid)
    {
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
        glDisable(GL_TEXTURE_2D);
    }
    else if(mode == texture)
    {
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_tex);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse_tex);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular_tex);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess_tex);
        // load the texture map
        loadTextureMap();
    }
    else if(mode == environment)
    {
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_tex);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse_tex);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular_tex);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess_tex);
        // load the environment map
        loadEnviromentMap();
    }

    glFlush();
    glutSwapBuffers();  // swap front/back framebuffer to avoid flickering
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.f, (float)w / h, 0.001f, 30.f);
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'm':
            displayMesh = !displayMesh;
            break;
        case 'l':
            lightsOn = !lightsOn;
            break;
        case 's':
            mode = solid;
            break;
        case 'e':
            mode = environment;
            break;
        case 't':
            mode = texture;
            break;
        case 27:
            glutLeaveMainLoop();
            break;
    }
}

void timer(int v)
{
    rotAngle += 1.f;

    glutPostRedisplay();
    glutTimerFunc(1000/nFPS, timer, v);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1000, 800);
    glutInitWindowPosition(2380, 100);
    glutCreateWindow("MP3 Teapot - sgupta80");

    // this is to make sure memory is cleared on program exit, so we
    // set it such that the main loop continues when we exit
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    std::cout << "OpenGL: " << glGetString(GL_VERSION) << ", GLSL: " << 
            glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    if(argv[1] == NULL)
    {
        cout << "No OBJ file specified." << endl;
        exit(EXIT_FAILURE);
    }

    if(argv[2] == NULL)
    {
        cout << "No texture map specified." << endl;
        exit(EXIT_FAILURE);
    }

    if(argv[3] == NULL)
    {
        cout << "No environment map specified." << endl;
        exit(EXIT_FAILURE);
    }

    init(argv[1], argv[2], argv[3]);

    glutDisplayFunc(display); 
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(100, timer, 0);
    glutMainLoop();

    delete teapot;
    delete texture_map;
    delete env_map;
    return 0;
}
