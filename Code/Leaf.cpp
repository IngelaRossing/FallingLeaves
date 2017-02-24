#include "Leaf.h"

// Function to generate a random double value between two values
float fRand(float fMin, float fMax)
{
    float f = (float)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}


Leaf::Leaf(): oldU(0.0f), oldV(0.0f), oldOmega(0.0f), oldAlpha(0.0f)
{
    oldX = fRand(-5.0f, 5.0f);      // these should be randoms
	oldY = fRand(-5.0f, 10.0f);
    z    = fRand(-5.0f, 5.0f);

    //oldAlpha = fRand(-1.0f, 1.0f);
    oldTheta = fRand(0.7f, 1.1f);
    rotZ = fRand(M_PI/2-1, M_PI/2);

	//Values needed for rotation
    kort = 5; //fRand(1.0f, 10.0f);    // Ortogonal friction
    kpar = kort/50;     // Parallel friction
    length = fRand(0.6f,0.9f);

    mesh.createBox(length, length, 0.00001f);
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


    //If the leaf moves too far down, push it up again.
    if(y < -10.0)
    {
        x = fRand(-7.0f, 7.0f);      // these should be randoms
        y = fRand(8.0f, 10.0f);
    }

    // Update variables for nest iteration
    oldU = u;
    oldV = v;
    oldX = x;
    oldY = y;
    oldAlpha = alpha;
    oldOmega = omega;
    oldTheta = theta;

}

void Leaf::draw(MatrixStack& mStack, GLint& location_MV, float time)
{
    mStack.push(); //Save the current matrix before performing multiplications


        mStack.rotX(0.4);
        mStack.translate(x, y, z);
        //mStack.rotY(std::min(0.0f, (float)sin(time)));
        mStack.rotZ(theta);
        mStack.rotZ(rotZ);

        glUniformMatrix4fv( location_MV, 1, GL_FALSE, mStack.getCurrentMatrix() );
        mesh.render(); //Draw the player

    mStack.pop(); //Restore the initial matrix
}

