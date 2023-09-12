//std
#include <stdio.h>
#include <stdlib.h>

//ext
#include "../external/cpp/inc/GL/glew.h"
#include "../external/cpp/inc/GL/freeglut.h"

//tensegrity
#include "inc/GPU/IBO.h"
#include "inc/GPU/VBO.h"
#include "inc/GPU/VAO.h"
#include "inc/GPU/Shader.h"
#include "inc/GPU/Program.h"

//data
static gpu::IBO ibo;
static gpu::VBO vbo;
static gpu::VAO vao;
static gpu::Program program;

void setup(void)
{
	//shaders
	program.m_vertex->m_path = "shd/base.vert";
	program.m_fragment->m_path = "shd/base.frag";
	//program
	program.setup();
	//buffers
	ibo.m_size = 6;
	vbo.m_vertices = 4;
	vbo.m_sizes = {3, 3};
	//setup
	ibo.setup();
	vbo.setup();
	vao.setup();
	//bind
	vao.bind();
	vbo.bind();
	ibo.bind();
	//enable
	vbo.enable();
	//allocate
	vbo.allocate();
	ibo.allocate();
	//data
	const float vbo_data[] = {
		-0.9, -0.9, 0, 0, 0, 1,
		+0.9, -0.9, 0, 0, 0, 1,
		+0.9, +0.9, 0, 0, 0, 1,
		-0.9, +0.9, 0, 0, 0, 1
	};
	const unsigned ibo_data[] = {
		0, 1, 2,
		0, 2, 3
	};
	vbo.transfer(vbo_data);
	ibo.transfer(ibo_data);
	//unbind
	vao.unbind();
	ibo.unbind();
	vbo.unbind();
}

void callback_display(void)
{
	//clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//bind
	vao.bind();
	vbo.bind();
	ibo.bind();
	//draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	//unbind
	vao.unbind();
	vbo.unbind();
	ibo.unbind();
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