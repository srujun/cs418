#ifndef MP3_H
#define MP3_H

#include "lodepng.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>

using namespace std;

/**
 * A class to represent a 3-dimensional point
 */
class Point3D
{
public:
    float x;
    float y;
    float z;

    Point3D()
    {
        x = y = z = 0.0;
    }

    Point3D(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Point3D(const Point3D & other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
    }

    /**
     * Overload various operators to make calculations easier.
     */

    Point3D & operator+=(const Point3D & rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    Point3D & operator-=(const Point3D & rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    const Point3D operator+(const Point3D & other) const
    {
        Point3D result = *this;
        result += other;
        return result;
    }

    const Point3D operator-(const Point3D & other) const
    {
        Point3D result = *this;
        result -= other;
        return result;
    }

    /**
     * Function to normalize the given point (if used as a vector)
     */
    void normalize()
    {
        x /= sqrt(x*x + y*y + z*z);
        y /= sqrt(x*x + y*y + z*z);
        z /= sqrt(x*x + y*y + z*z);
    }

    /*
     * Computes cross product with another vector
     */
    Point3D cross(Point3D other)
    {
        float compx = y * other.z - z * other.y;
        float compy = x * other.z - z * other.x;
        float compz = x * other.y - y * other.x;
        return Point3D(compx, -compy, compz);
    }

    /*
     * Computes cylindrical coordinates of this point
     */
    Point3D getCylindrical(float maxY) const
    {
        float theta = atan2(z, x);
        float xval = (theta + M_PI) / 2 * M_PI;
        float yval = y / maxY;
        float zval = 0.0f;

        return Point3D(xval, yval, zval);
    }
};

/*
 * Redefine Point3D to also be used as a Vertex
 */
typedef Point3D Vertex;

/*
 * Class to represent a 3D model's various faces
 */
class Face
{
public:
    unsigned int vidx0;
    unsigned int vidx1;
    unsigned int vidx2;
    Point3D normal;

    Face(int i0, int i1, int i2)
    {
        vidx0 = i0;
        vidx1 = i1;
        vidx2 = i2;
    }

    void setNormal(Vertex v0, Vertex v1, Vertex v2)
    {
        Vertex a = v1 - v0;
        Vertex b = v2 - v0;
        normal = a.cross(b);
    }
};


/*
 * Class to represent the input Teapot model
 */
class Teapot
{
public:
    vector<Vertex> vertices;
    vector<Point3D> vertexNormals;
    vector<Face> faces;
    float maxYval;

    /**
     * Constructor to load the teapot .obj file
     */
    Teapot(string filename)
    {
        readObj(filename);

        // create empty vertex normals for every vertex
        vertexNormals.resize(vertices.size());

        // computes the per-face normal and then per-vertex normals
        computeNormals();
    }

private:
    /**
     * This function reads the .obj specified by filename. It reads
     * line by line and stores each vertex or face in their respective
     * vector arrays.
     */
    void readObj(string filename)
    {
        ifstream objFile;
        objFile.open(filename.c_str(), ifstream::in);

        if(!objFile.is_open())
        {
            cout << "Failed to open " << filename << "!" << endl;
            exit(EXIT_FAILURE);
        }

        cout << "Found " << filename << "! Parsing..." << endl;

        maxYval = 0.0f;
        string line;
        while(getline(objFile, line))
        {
            // use input string stream abstractions to extract
            // information in every line of the obj file
            char linetype;
            istringstream ss(line);
            ss >> linetype;

            if(linetype == 'v')
            {
                float x, y, z;
                ss >> x >> y >> z;
                Vertex v(x, y, z);
                vertices.push_back(v);

                if(y > maxYval)
                    maxYval = y;
            }
            else if(linetype == 'f')
            {
                int idx0, idx1, idx2;
                ss >> idx0 >> idx1 >> idx2;
                Face f(idx0-1, idx1-1, idx2-1);
                faces.push_back(f);
            }
        }

        objFile.close();
    }

    /**
     * This function first computes the per-face normals by using a cross
     * product, and then it computes the per-vertex normals by accumulating
     * the normals of adjacent faces of every vertex, and then normalizing
     * the accumulated quantity.
     */
    void computeNormals()
    {
        // loop through all the faces to compute per-face normals
        for(unsigned int i = 0; i < faces.size(); i++)
        {
            Vertex v0 = vertices[faces[i].vidx0];
            Vertex v1 = vertices[faces[i].vidx1];
            Vertex v2 = vertices[faces[i].vidx2];

            // set this face's normal and then normalize it
            faces[i].setNormal(v0, v1, v2);
            faces[i].normal.normalize();

            // add this face's normal to the normals of the
            // 3 adjacent vertices

            vertexNormals[faces[i].vidx0] += faces[i].normal;
            vertexNormals[faces[i].vidx1] += faces[i].normal;
            vertexNormals[faces[i].vidx2] += faces[i].normal;
        }

        // normalize all the vertex-normals
        for(auto & vtxnorm : vertexNormals)
            vtxnorm.normalize();
    }
};

/*
 * Class to represent an input image, which may be used as textures.
 * I have used lodePNG to do image reading and parsing. The data is
 * then stored in a vector of bytes (chars).
 */
class Image
{
private:
    vector<unsigned char> image;
    unsigned int width, height;

public:
    Image(string filename)
    {
        unsigned int error = lodepng::decode(image, width, height, filename.c_str());

        if(error != 0)
        {
            cout << "Error loading image " << filename << endl;
            cout << "lodePNG: " << lodepng_error_text(error) << endl;
            return;
        }

    }

    const unsigned char * getBytes() const
    {
        return &image[0];
    }

    unsigned int getWidth() const
    {
        return width;
    }

    unsigned int getHeight() const
    {
        return height;
    }
};

#endif
