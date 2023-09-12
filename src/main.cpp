//std
#include <stdio.h>
#include <stdlib.h>

//ext
#include "../external/cpp/inc/GL/glew.h"
#include "../external/cpp/inc/GL/freeglut.h"

//gpu
#include "inc/GPU/IBO.hpp"
#include "inc/GPU/VBO.hpp"
#include "inc/GPU/VAO.hpp"
#include "inc/GPU/Shader.hpp"
#include "inc/GPU/Program.hpp"

//draw
#include "inc/draw/Cable.hpp"
#include "inc/draw/Canvas.hpp"

//data
static gpu::IBO ibo;
static gpu::VBO vbo;
static gpu::VAO vao;
static gpu::Program program;

static draw::Canvas canvas;

void setup(void)
{
	//shaders
	program.m_vertex->m_path = "shd/base.vert";
	program.m_fragment->m_path = "shd/base.frag";
	//program
	program.setup();
	//canvas
	canvas.m_objects.push_back(new draw::Cable);
	//buffers
	canvas.setup();
	vbo.m_sizes = {3, 3};
	vbo.m_vertices = canvas.m_vertices;
	ibo.m_size = canvas.m_points + 2 * canvas.m_lines + 3 * canvas.m_triangles;
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
	canvas.draw();
	vbo.transfer(canvas.m_vbo_data);
	ibo.transfer(canvas.m_ibo_data);
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
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
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