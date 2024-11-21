#ifdef WIN32
#include <windows.h>
#include <GL/glut.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <GL/glut.h>
#endif

#include "Ray.h"
#include "Ponto.h"
#include "Plane.cpp"
#include <iostream>

bool Ray::b_intersectPlane(Plane plane)
{
    float denominator = Ponto::dotProduct(&plane.n, &this->direction);
    if (abs(denominator) <= 1e-4f)
    {
        // std::cout << "MENOR QUE ZERO\n";
        length = 0;
        return false;
    }

    if (-denominator <= 1e-4f)
    {
        length = 0;
        return false;
    }

    length = -(Ponto::dotProduct(&plane.n, &this->origin) + plane.d) / denominator;

    if (length <= 1e-4f)
    {
        length = 0;
        return false;
    }

    std::cout << "length " << length << "\n";

    end = origin + (direction * length);

    std::cout << "end " << end.x << "," << end.y << "," << end.z << "\n";

    return true;
}

void Ray::draw()
{
    glColor3f(1.0f, 1.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex3f(direction.x, direction.y, direction.z);
        glVertex3f(end.x, end.y, end.z);
    glEnd();
}