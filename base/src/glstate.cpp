#define NOMINMAX
#include "glstate.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "util.hpp"
#include <iostream>
#include <chrono>  // for high_resolution_clock
#include <cmath>

int GLState::width = 800;
int GLState::height = 800;

// Constructor
GLState::GLState() :
	w(1), h(1),
	fovy(45.0f),
	camCoords(0.0f, 0.0f, 2.0f),
	objMode(OBJMODE_MESH),  // OBJMODE_TETRAHEDRON
	// states of the tetrahedron:
	shader(0),
	xformLoc(0),
	vao(0),
	vbuf(0),
	ibuf(0),
	vcount(0),
	// states of the platform:
	lineShader(0),
	lineXformLoc(0),
	lineVao(0),
	lineVbuf(0),
	lineIbuf(0),
	time_uniform_loc(-1),
	iResolution_uniform_loc(-1)
{
}

// Destructor
GLState::~GLState() {
	// Release OpenGL resources
	if (shader)	glDeleteProgram(shader);
	if (vao)	glDeleteVertexArrays(1, &vao);
	if (vbuf)	glDeleteBuffers(1, &vbuf);
	if (ibuf)	glDeleteBuffers(1, &ibuf);

	if (lineShader)	glDeleteProgram(lineShader);
	if (lineVao)	glDeleteVertexArrays(1, &lineVao);
	if (lineVbuf)	glDeleteBuffers(1, &lineVbuf);
	if (lineIbuf)	glDeleteBuffers(1, &lineIbuf);
}

// Called when OpenGL context is created (some time after construction)
void GLState::initializeGL() {
	// General settings
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);

	// Initialize OpenGL state
	initShaders();
	initLineGeometry();
}

// Called when window requests a screen redraw
void GLState::paintGL() {
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// #################### First pass: draw the quad ####################
	// Set shader to draw with
	glUseProgram(lineShader);

	glm::mat4 lineXform = glm::mat4(1.0f);

	// Send transform matrix to shader
	glUniformMatrix4fv(lineXformLoc, 1, GL_FALSE, glm::value_ptr(lineXform));

	// Send the current time to shader
	glUniform1f(time_uniform_loc, cur_time);
	glUniform2i(iResolution_uniform_loc, width, height);

	// Use our vertex format and buffers
	glBindVertexArray(lineVao);
	// Draw the geometry
	glLineWidth((GLfloat)10.0f);  // define the width of the bar
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);  // NOTE: use GL_LINE will fail to draw the line
	// Cleanup state
	glBindVertexArray(0);

	glUseProgram(0);  // end second pass
}

// Called when window is resized
void GLState::resizeGL(int w, int h) {
	// Tell OpenGL the new dimensions of the window
	width = w;
	height = h;
	glViewport(0, 0, w, h);
}


void GLState::update_time(float time) {
	cur_time = time;
}


// Create shaders and associated state
void GLState::initShaders() {
	// Compile and link shader files
	std::vector<GLuint> lineShaders;
	lineShaders.push_back(compileShader(GL_VERTEX_SHADER, "shaders/v.glsl"));
	lineShaders.push_back(compileShader(GL_FRAGMENT_SHADER, "shaders/f.glsl"));
	lineShader = linkProgram(lineShaders);
	for (auto s : lineShaders)
		glDeleteShader(s);
	lineShaders.clear();

	lineXformLoc			= glGetUniformLocation(lineShader, "xform");
	time_uniform_loc		= glGetUniformLocation(lineShader, "time");
	iResolution_uniform_loc = glGetUniformLocation(lineShader, "iResolution");
}

void GLState::initLineGeometry() {
	// Vertices
	std::vector<Vertex> verts{
		// Position                 // Normal
	  { {  -1.0f,  -1.0f,  0.0f }, {  0.0f, 0.0f, 0.0f }, },	// v0
	  { {  1.0f,  -1.0f,  0.0f }, {  0.0f, 0.0f, 0.0f }, },	// v1
	  { {  -1.0f,  1.0f,  0.0f }, {  0.0f, 0.0f, 0.0f }, },	// v2
	  { {  1.0f,  1.0f,  0.0f }, {  0.0f, 0.0f, 0.0f }, },	// v3
	};

	// Triangle indices
	std::vector<GLuint> inds{
		0, 1, 2,
		2, 1, 3,
	};

	// Create vertex array object
	glGenVertexArrays(1, &lineVao);
	glBindVertexArray(lineVao);

	// Create OpenGL buffers for vertex and index data
	glGenBuffers(1, &lineVbuf);
	glBindBuffer(GL_ARRAY_BUFFER, lineVbuf);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW);
	glGenBuffers(1, &lineIbuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineIbuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(GLuint), inds.data(), GL_STATIC_DRAW);

	// Specify vertex attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)sizeof(glm::vec3));

	// Cleanup state
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
