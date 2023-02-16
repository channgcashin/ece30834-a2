#include <iostream>
#include <memory>
#include <filesystem>
#include <algorithm>
#include "glstate.hpp"
#include <GL/freeglut.h>
namespace fs = std::filesystem;

std::vector<std::string> meshFilenames;		// Paths to .obj files to load

// OpenGL state
std::unique_ptr<GLState>	glState;

// Initialization functions
void	initGLUT(int* argc, char** argv);
void	findObjFiles();

// Callback functions
void	display();
void	reshape(GLint width, GLint height);
void	keyRelease(unsigned char key, int x, int y);
void	mouseBtn(int button, int state, int x, int y);
void	idle();
void	cleanup();

// Program entry point
int main(int argc, char** argv) {
	try {
		// Create the window and menu
		initGLUT(&argc, argv);
		// Initialize OpenGL (buffers, shaders, etc.)
		glState = std::unique_ptr<GLState>(new GLState());
		glState->initializeGL();

	} catch (const std::exception& e) {
		// Handle any errors
		std::cerr << "Fatal error: " << e.what() << std::endl;
		cleanup();
		return -1;
	}

	// Execute main loop
	glutMainLoop();

	return 0;
}

// Setup window and callbacks
void initGLUT(int* argc, char** argv) {
	// Set window and context settings
	glutInit(argc, argv);
	glutInitWindowSize(GLState::width, GLState::height);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	// Create the window
	glutCreateWindow("FreeGLUT Window");

	// GLUT callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardUpFunc(keyRelease);
	glutMouseFunc(mouseBtn);
	glutIdleFunc(idle);
	glutCloseFunc(cleanup);
}

void findObjFiles() {
	// Search the models/ directory for any file ending in .obj
	fs::path	modelsDir								   = "models";
	for (auto& di : fs::directory_iterator(modelsDir)) {
		if (di.is_regular_file() && di.path().extension() == ".obj")
			meshFilenames.push_back(di.path().string());
	}
	std::sort(meshFilenames.begin(), meshFilenames.end());
}

// Called whenever a screen redraw is requested
void display() {
	// Tell the GLState to render the scene
	glState->paintGL();

	// Scene is rendered to the back buffer, so swap the buffers to display it
	glutSwapBuffers();
}

// Called when the window is resized
void reshape(GLint width, GLint height) {
	// Tell OpenGL the new window size
	glState->resizeGL(width, height);
}

// Called when a mouse button is pressed or released
void mouseBtn(int button, int state, int x, int y) {
}

static auto start = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());	// record start time

// Called when there are no events to process
void idle() {
	// NOTE: anything that happens every frame (e.g. movement) should be done here
	// Be sure to call glutPostRedisplay() if the screen needs to update as well
	auto	finish	= std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());	// record end time
	auto	elapsed = static_cast<float>((finish - start).count());

	if ((int)elapsed % (int)(1000.0/60.0) == 0) {
		glState->update_time(elapsed);
		glutPostRedisplay();
	}
}

// Called when the window is closed or the event loop is otherwise exited
void cleanup() {
	// Delete the GLState object, calling its destructor,
	// which releases the OpenGL objects
	glState.reset(nullptr);
}


void keyRelease(unsigned char key, int x, int y) {
	switch (key) {
		case 'r':				// Reload the shader
			// TODO 1, reload the shader program, hint: find an appropriate function in the GLState class.
			glState->initializeGL();
			printf("Reload Shader. \n");
			break;
	}
}