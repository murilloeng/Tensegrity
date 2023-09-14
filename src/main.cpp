//std
#include <string>
#include <stdio.h>
#include <stdlib.h>

//ext
#include "../external/cpp/inc/GL/glew.h"
#include "../external/cpp/inc/GL/freeglut.h"

//tensegrity
#include "inc/gpu.hpp"

//data
static const unsigned nc = 5;
static const unsigned nd = 80;
static const double r = 3.00e-01;
static const double e = 1.50e-01;
static const double ht = 1.00e+00;
static const double hc = 5.00e-01;
static GLuint vao, vbo, ibo, program;

static const double hr = (ht + hc) / 2;

void move(float* vbo_data, unsigned start, unsigned count, const float* dx)
{
	for(unsigned i = start; i < start + count; i++)
	{
		for(unsigned j = 0; j < 3; j++)
		{
			vbo_data[3 * i + j] += dx[j];
		}
	}
}

//draw
void draw_rigid(void)
{
	//data
	float vbo_data[3 * (nd + 4)];
	unsigned ibo_data[3 * nd + 4];
	//bind
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	//center
	vbo_data[3 * 0 + 0] = 0;
	vbo_data[3 * 0 + 1] = 0;
	vbo_data[3 * 0 + 2] = 0;
	//boundary
	for(unsigned i = 0; i < nd; i++)
	{
		vbo_data[3 * (i + 1) + 2] = 0;
		vbo_data[3 * (i + 1) + 0] = r * cos(2 * M_PI * i / nd);
		vbo_data[3 * (i + 1) + 1] = r * sin(2 * M_PI * i / nd);
	}
	//pole 1
	vbo_data[3 * (nd + 1) + 0] = e;
	vbo_data[3 * (nd + 1) + 1] = 0;
	vbo_data[3 * (nd + 1) + 2] = 0;
	//pole 2
	vbo_data[3 * (nd + 2) + 0] = e;
	vbo_data[3 * (nd + 2) + 1] = 0;
	vbo_data[3 * (nd + 2) + 2] = hr;
	//pole 3
	vbo_data[3 * (nd + 3) + 0] = 0;
	vbo_data[3 * (nd + 3) + 1] = 0;
	vbo_data[3 * (nd + 3) + 2] = hr;
	//indices
	for(unsigned i = 0; i < nd; i++)
	{
		ibo_data[3 * i + 0] = 0;
		ibo_data[3 * i + 1] = i + 1;
		ibo_data[3 * i + 2] = i + 2;
	}
	//transfer
	glBufferData(GL_ARRAY_BUFFER, 6 * (nd + 3) * sizeof(float), vbo_data, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (3 * nd + 4) * sizeof(unsigned), ibo_data, GL_STATIC_DRAW);
	//unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void setup_buffers(GLuint& vao, GLuint& vbo, GLuint& ibo)
{
	//buffers
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);
	glGenVertexArrays(1, &vao);
	//bind
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	//enable
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (unsigned*) (0 * sizeof(float)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (unsigned*) (3 * sizeof(float)));
	//draw
	draw_rigid();
	//unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void setup(void)
{
	setup_buffers(vao, vbo, ibo);
	setup_program(program, "shd/model.vert", "shd/model.frag");
}

void callback_idle(void)
{
	return;
}
void callback_display(void)
{
	//clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//bind
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	//draw
	glDrawElements(GL_TRIANGLES, 3 * nd, GL_UNSIGNED_INT, nullptr);
	//unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//swap
	glutSwapBuffers();
}
void callback_reshape(GLint width, GLint height)
{
	//viewport
	glViewport(0, 0, width, height);
	//update
	glutPostRedisplay();
}
void callback_mouse(int button, int state, int x1, int x2)
{
	return;
}
void callback_motion(int x1, int y2)
{
	return;
}
void callback_keyboard(unsigned char key, int x1, int y2)
{
	if(key == 27)
	{
		glutDestroyWindow(glutGetWindow());
	}
}

int main(int argc, char** argv)
{
	//glut
	glutInit(&argc, argv);
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(0, 0);
	glutInitContextVersion(4, 6);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	//window
	glutCreateWindow("Tensegrity Chair");
	//glew
	GLenum status = glewInit();
	if(status != GLEW_OK)
	{
		printf("GLEW init failed: %s!\n", glewGetErrorString(status));
		exit(EXIT_FAILURE);
	}
	//setup
	setup();
	//callbacks
	glutIdleFunc(callback_idle);
	glutMouseFunc(callback_mouse);
	glutMotionFunc(callback_motion);
	glutDisplayFunc(callback_display);
	glutReshapeFunc(callback_reshape);
	glutKeyboardFunc(callback_keyboard);
	//loop
	glutMainLoop();
	//return
	return EXIT_SUCCESS;
}