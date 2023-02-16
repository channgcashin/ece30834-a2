#ifndef GLSTATE_HPP
#define GLSTATE_HPP

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "gl_core_3_3.h"
#include "mesh.hpp"

// Manages OpenGL state, e.g. camera transform, objects, shaders
class GLState {
public:
	GLState();
	~GLState();
	// Disallow copy, move, & assignment
	GLState(const GLState& other) = delete;
	GLState& operator=(const GLState& other) = delete;
	GLState(GLState&& other) = delete;
	GLState& operator=(GLState&& other) = delete;

	void initShaders();

	// Callbacks
	void initializeGL();
	void paintGL();
	void resizeGL(int w, int h);
	void update_time(float time);


	// Per-vertex attributes
	struct Vertex {
		glm::vec3 pos;		// Position
		glm::vec3 norm;		// Normal
	};

	// window size:
	static int width, height;  // 600 by default

protected:
	// Initialization
	void initLineGeometry();

	// Camera state
	int w, h;		// Width and height of the window
	float fovy;				// Vertical field of view in degrees
	glm::vec3 camCoords;	// Camera spherical coordinates
	glm::vec2 initMousePos;	// Initial mouse position on click

	// Object state
	enum ObjMode {
		OBJMODE_MESH,				// Show a given obj file
	};
	ObjMode objMode;				// Which object state are we in
	std::string meshFilename;		// Name of the obj file being shown
	std::unique_ptr<Mesh> mesh;		// Pointer to mesh object

	// OpenGL states of the tetrahedron
	GLuint shader;		// GPU shader program
	GLuint xformLoc;	// Transformation matrix location
	GLuint vao;			// Vertex array object
	GLuint vbuf;		// Vertex buffer
	GLuint ibuf;		// Index buffer
	GLsizei vcount;		// Number of indices to draw

	// OpenGL states of the platform
	GLuint lineShader;		// GPU shader program
	GLuint lineXformLoc;	// vertical position of the bar on screen
	GLuint lineVao;			// Vertex array object
	GLuint lineVbuf;		// Vertex buffer
	GLuint lineIbuf;		// Index buffer

	// A2 Related
	float cur_time;
	GLuint time_uniform_loc;

	glm::ivec2 iResolution;
	GLuint iResolution_uniform_loc;
};

#endif
