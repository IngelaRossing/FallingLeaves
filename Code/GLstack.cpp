/*
 * A C++ framework for OpenGL programming in TNM061 for MT1 2014.
 *
 * This is a small, limited framework, designed to be easy to use
 * for students in an introductory computer graphics course in
 * the second year of a M Sc curriculum. It uses custom code
 * for some things that are better solved by external libraries
 * like GLEW and GLM, but the emphasis is on simplicity and
 * readability, not generality.
 * For the window management, GLFW 3.0 is used for convenience.
 * The framework should work in Windows, MacOS X and Linux.
 * Some Windows-specific stuff for extension loading is still
 * here. GLEW could have been used instead, but for clarity
 * and minimal dependence on other code, we rolled our own extension
 * loading for the things we need. That code is short-circuited on
 * platforms other than Windows. This code is dependent only on
 * the GLFW and OpenGL libraries. OpenGL 3.3 or higher is required.
 *
 * Author: Stefan Gustavson (stegu@itn.liu.se) 2013-2014
 * This code is in the public domain.
 */

using namespace std;

// System utilities
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI (3.141592653589793)
#endif

// In MacOS X, tell GLFW to include the modern OpenGL headers.
// Windows does not want this, so we make this Mac-only.
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#endif

// In Linux, tell GLFW to include the modern OpenGL functions.
// Windows does not want this, so we make this Linux-only.
#ifdef __linux__
#define GL_GLEXT_PROTOTYPES
#endif

// GLFW 3.x, to handle the OpenGL window
#include <GLFW/glfw3.h>

// Windows installations usually lack an up-to-date OpenGL extension header,
// so make sure to supply your own, or at least make sure it's of a recent date.
#ifdef __WIN32__
#include <GL/glext.h>
#endif

// Headers for the other source files that make up this program.
#include "tnm061.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "TriangleSoup.hpp"
#include "Rotator.hpp"
#include "MatrixStack.hpp"

/*
 * setupViewport() - set up the OpenGL viewport.
 * This should be done for each frame, to handle window resizing.
 * The "proper" way would be to set a "resize callback function"
 * using glfwSetWindowSizeCallback() and do these few operations
 * only when something changes, but let's keep it simple.
 * Besides, we want to change P when the aspect ratio changes.
 * A callback function would require P to be changed indirectly
 * in some manner, which is somewhat awkward in this case.
 */
void setupViewport(GLFWwindow *window, GLfloat *P) {

    int width, height;

    // Get window size. It may start out different from the requested
    // size, and will change if the user resizes the window.
    glfwGetWindowSize( window, &width, &height );

    // Ugly hack: adjust perspective matrix for non-square aspect ratios
    P[0] = P[5]*height/width;

    // Set viewport. This is the pixel rectangle we want to draw into.
    glViewport( 0, 0, width, height ); // The entire window
}


/*
 * main(argc, argv) - the standard C entry point for the program
 */
int main(int argc, char *argv[]) {

	TriangleSoup leaf;
    Texture leafTexture;
    Shader leafShader;

 	GLint location_time, location_MV, location_P, location_tex; // Shader uniforms
    float time;
	double fps = 0.0;

    MatrixStack MVstack; // The matrix stack we are going to use to set MV

    const GLFWvidmode *vidmode;  // GLFW struct to hold information about the display
	GLFWwindow *window;    // GLFW struct to hold information about the window

	MouseRotator rotator;

    // Initialise GLFW
    glfwInit();

    // Determine the desktop size
    vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	// Make sure we are getting a GL context of at least version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Exclude old legacy cruft from the context. We don't need it, and we don't want it.
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a square window (aspect 1:1) to fill half the screen height
    window = glfwCreateWindow(vidmode->height/2, vidmode->height/2, "GLprimer", NULL, NULL);
    if (!window)
    {
        glfwTerminate(); // No window was opened, so we can't continue in any useful way
        return -1;
    }

    // Make the newly created window the "current context" for OpenGL
    // (This step is strictly required, or things will simply not work)
    glfwMakeContextCurrent(window);

	rotator.init(window);

    // Load the extensions for GLSL - note that this has to be done
    // *after* the window has been opened, or we won't have a GL context
    // to query for those extensions and connect to instances of them.
    tnm061::loadExtensions();

    printf("GL vendor:       %s\n", glGetString(GL_VENDOR));
    printf("GL renderer:     %s\n", glGetString(GL_RENDERER));
    printf("GL version:      %s\n", glGetString(GL_VERSION));
    printf("Desktop size:    %d x %d pixels\n", vidmode->width, vidmode->height);

    glfwSwapInterval(0); // Do not wait for screen refresh between frames

	// Perspective projection matrix
	// This is the standard gluPerspective() form of the
    // matrix, with d=4, near=3, far=7 and aspect=1.
    GLfloat P[16] = {
		4.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 4.0f, 0.0f, 0.0f,
  		0.0f, 0.0f, -2.5f, -1.0f,
		0.0f, 0.0f, -10.5f, 0.0f
	};

    // Intialize the matrix to an identity transformation
    MVstack.init();

	// Create geometry for rendering
	leaf.createBox(0.3f, 0.3f, 0.00001f);
	// soupReadOBJ(&myShape, MESHFILENAME);
	leaf.printInfo();

	// Create a shader program object from GLSL code in two files
	leafShader.createShader("vertexshader.glsl", "fragmentshader.glsl");

	glEnable(GL_TEXTURE_2D);

	//Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Read the texture data from file and upload it to the GPU
	leafTexture.createTexture("textures/testleaf3.tga");

	location_MV = glGetUniformLocation( leafShader.programID, "MV" );
	location_P = glGetUniformLocation( leafShader.programID, "P" );
	location_time = glGetUniformLocation( leafShader.programID, "time" );
	location_tex = glGetUniformLocation( leafShader.programID, "tex" );

	/* NEEDED VARIABLES for simple fall in one dimension with drag (Euler) */
	//Starting values
	float oldU = 0.0f;      // u is the velocity in X
    float oldV = 0.0f;      // v is the velocity in Y
    float oldX = 0.0f;      // position x
	float oldY = 5.0f;      // position y
	float oldTime = 0.0f;
	float oldAngVelocity = 0;
	float oldAlpha = 0;            // The direction the leaf is moving (radians)
	float oldAngle = 1;            // The leaf's orientation (radians)
	float newU, newV, newX, newY, newTime, newAngVelocity, newAlpha, newAngle;
//	float C = 0.9;              //Coefficient for air resistance
//	float rho = 1.3;           //Air density (kg/m^3)
//	float A = 0.01;            //Effective area (m^2)
//	float k = 0.5*C*rho*A;     //Air resistance constant
//	float m = 0.003;           //Mass of the leaf (kg)
	float g = 9.82;            //Gravitational acceleration
	float h = 0.01;            //Step length

	//Values needed for rotation
	float rho = 0.05;    // Value between 1 and 0. Relationship between leaf density and air density
    float kort = 10;    // Ortogonal friction
    float kpar = 0.1;  // Parallel friction
    float lang = 0.07;

    // Main loop
    while(!glfwWindowShouldClose(window))
    {
        // Calculate and update the frames per second (FPS) display
        fps = tnm061::displayFPS(window);

		// Set the clear color and depth, and clear the buffers for drawing
        glClearColor(0.4f, 0.87f, 0.95f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST); // Use the Z buffer
		glDisable(GL_CULL_FACE);  // Use back face culling
		glCullFace(GL_BACK);

        // Set up the viewport
        setupViewport(window, P);

		// Handle mouse input
		rotator.poll(window);
		//printf("phi = %6.2f, theta = %6.2f\n", rotator.phi, rotator.theta);

		// Activate our shader program.
		glUseProgram( leafShader.programID );

        // Copy the projection matrix P into the shader.
		glUniformMatrix4fv( location_P, 1, GL_FALSE, P );

        // Tell the shader to use texture unit 0.
		glUniform1i ( location_tex , 0);

		// Update the uniform time variable.
		time = (float)glfwGetTime(); // Needed later as well
        glUniform1f( location_time, time );

        // Draw the scene
        MVstack.push(); // Save the initial, untouched matrix

            // Modify MV according to user input
            // First, do the view transformations ("camera motion")
            MVstack.translate(0.0f, 0.0f, -5.0f);
            MVstack.scale(0.2);
            MVstack.rotX(rotator.theta);
            MVstack.rotY(rotator.phi);

            // Then, do the model transformations ("object motion")
            MVstack.push(); // Save the current matrix on the stack

                    h = (time - oldTime);

                    //cout << "h =" << h << endl; //What is the step length?

                    if(h < 0.15)
                    {
                        /* Do necessary calculations (Euler)*/

                        newU = oldU + (-(kort*sin(oldAngle)*sin(oldAngle) + kpar*cos(oldAngle)*cos(oldAngle))*oldU
                                + (kort - kpar)*sin(oldAngle)*cos(oldAngle)*oldV
                                - M_PI*rho*(oldU*oldU + oldV*oldV)*cos(oldAlpha + oldAngle)*cos(oldAlpha))*h;

                        newV = oldV + ((kort - kpar)*sin(oldAngle)*cos(oldAngle)*oldU
                                - (kort*sin(oldAngle)*sin(oldAngle) + kpar*cos(oldAngle)*cos(oldAngle))*oldV
                                + M_PI*rho*(oldU*oldU + oldV*oldV)*cos(oldAlpha + oldAngle)*sin(oldAlpha) - g)*h;

                        newAlpha = atan(newU/newV); // New movement direction

                        newAngVelocity = oldAngVelocity +
                                         (-kort*oldAngVelocity - (3*M_PI*rho*(oldU*oldU + oldV*oldV)/lang)*cos(oldAlpha + oldAngle)*sin(oldAlpha + oldAngle))*h;

                        newAngle = oldAngle + oldAngVelocity*h;

                        // The position becomes:
                        newX = oldX + oldU*h;
                        newY = oldY + oldV*h;

                        cout << "x: " << newX << "  y: " << newY << endl;


                        //When does the leaf reach the ground?
                        if(round(newY) == 0)
                            cout << time << endl;
                    }

                    /* NOTE: F�r flera l�v kommer det beh�vas en vektor eller array inneh�llande dess positioner och hastigheter */

                    // One leaf (
                    MVstack.rotY(time);
                    MVstack.rotX(0.2);  //Denna rotation g�r s� att l�vet "singlar" ner
                    MVstack.translate(newX, newY, 0.0f);
                    MVstack.rotZ(newAngle);
                    glUniformMatrix4fv( location_MV, 1, GL_FALSE, MVstack.getCurrentMatrix() );
                    // Render the geometry to draw the sun
                    glBindTexture(GL_TEXTURE_2D, leafTexture.texID);
                    leaf.render();

                    /* Update variables for nest iteration */
                    oldU = newU;
                    oldV = newV;
                    oldX = newX;
                    oldY = newY;
                    oldAlpha = newAlpha;
                    oldAngVelocity = newAngVelocity;
                    oldAngle = newAngle;

                    oldTime = time;


            MVstack.pop(); // Restore the matrix we saved above

        MVstack.pop(); // Restore the initial, untouched matrix

		// Play nice and deactivate the shader program
		glUseProgram(0);

		// Swap buffers, i.e. display the image and prepare for next frame.
        glfwSwapBuffers(window);

		// Poll events (read keyboard and mouse input)
		glfwPollEvents();

        // Exit if the ESC key is pressed (and also if the window is closed).
        if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
          glfwSetWindowShouldClose(window, GL_TRUE);
        }

    }

    // Close the OpenGL window and terminate GLFW.
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
