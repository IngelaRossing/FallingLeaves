#include "Leaf.h"

Leaf::Leaf(): oldU(0.0f), oldV(0.0f), oldOmega(0.0f), oldAlpha(0.0f), oldTheta(1.0f)
{
    oldX = 1.0f;      // these should be randoms
	oldY = 5.0f;
    z    = 0.0f;

	//Values needed for rotation
    kort = 10.0f;    // Ortogonal friction
    kpar = 0.1f;     // Parallel friction
    length = 0.07f;

    mesh.createBox(0.3f, 0.3f, 0.00001f);
}

void Leaf::update(float h)
{
    u = oldU + (-(kort*sin(oldTheta)*sin(oldTheta) + kpar*cos(oldTheta)*cos(oldTheta))*oldU
           + (kort - kpar)*sin(oldTheta)*cos(oldTheta)*oldV
           - M_PI*RHO*(oldU*oldU + oldV*oldV)*cos(oldTheta + oldTheta)*cos(oldTheta))*h;

    v = oldV + ((kort - kpar)*sin(oldTheta)*cos(oldTheta)*oldU
           - (kort*cos(oldTheta)*cos(oldTheta) + kpar*sin(oldTheta)*sin(oldTheta))*oldV
           + M_PI*RHO*(oldU*oldU + oldV*oldV)*cos(oldAlpha + oldTheta)*sin(oldAlpha) - g)*h;

    alpha = atan(u/v); // New movement direction

    omega = oldOmega +
           (-kort*oldOmega - (3*M_PI*RHO*(oldU*oldU + oldV*oldV)/length)*cos(oldAlpha + oldTheta)*sin(oldAlpha + oldTheta))*h;

    theta = oldTheta + oldOmega*h;

    // The position becomes:
    x = oldX + oldU*h;
    y = oldY + oldV*h;

    // Update variables for nest iteration
    oldU = u;
    oldV = v;
    oldX = x;
    oldY = y;
    oldAlpha = alpha;
    oldOmega = omega;
    oldTheta = theta;
}

void Leaf::draw(MatrixStack& mStack, GLint& location_MV)
{
    mStack.push(); //Save the current matrix before performing multiplications

        mStack.rotX(0.4);
        mStack.translate(x, y, z);
        mStack.rotZ(theta);

        glUniformMatrix4fv( location_MV, 1, GL_FALSE, mStack.getCurrentMatrix() );
        mesh.render(); //Draw the player

    mStack.pop(); //Restore the initial matrix
}
