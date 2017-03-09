#ifndef LEAF_H
#define LEAF_H

#ifndef M_PI
#define M_PI (3.141592653589793)
#endif

#include "TriangleSoup.hpp"
#include "matrixStack.hpp"
#include "Shader.hpp"
#include <ctime>
#include <algorithm>    // std::min

/* This is a class describing a single leaf */


class Leaf
{
    public:
        Leaf();

        //void update(float h);
        void update(float h, GLFWwindow *window);
        void draw(MatrixStack& mStack, GLint& location_MV, float time);

    private:
        //Gravity and relative density are the same for all leaves
        const float g = 9.82;
        const float RHO = 0.1;

        float length, kort, kpar;   // Characteristics of the leaf. kort - ortogonal friction, kpar - parallel friction
        float x, y, z, oldX, oldY;  // Leaf position
        float u, v, oldU, oldV;     // Leaf velocity (just x and y)
        float theta, oldTheta;      // Leaf orientation angle (radians)
        float omega, oldOmega;      // Angular velocity, derivative of theta
        float alpha, oldAlpha;      // The direction the leaf is traveling
        float rotZ;                 // Starting angle for the leaf in z
        float windX, windY;

        TriangleSoup mesh;
};

#endif // LEAF_H
