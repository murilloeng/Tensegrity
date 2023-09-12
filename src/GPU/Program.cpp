//os
#ifdef _WIN32
#include <Windows.h>
#endif

//ext
#include "../external/cpp/inc/GL/glew.h"

//tensegrity
#include "inc/GPU/Shader.h"
#include "inc/GPU/Program.h"

namespace gpu
{
	//constructors
	Program::Program(void) : m_vertex(new Shader), m_fragment(new Shader)
	{
		m_vertex->m_type = GL_VERTEX_SHADER;
		m_fragment->m_type = GL_FRAGMENT_SHADER;
	}

	//destructor
	Program::~Program(void)
	{
		delete m_vertex;
		delete m_fragment;
		if(glIsProgram(m_id))
		{
			glDeleteProgram(m_id);
		}
	}

	//setup
	void Program::use(void)
	{
		glUseProgram(m_id);
	}
	void Program::setup(void)
	{
		//create
		m_id = glCreateProgram();
		//check
		if(m_id == 0)
		{
			fprintf(stderr, "Error creating shader program!\n");
			exit(EXIT_FAILURE);
		}
		//shaders
		m_vertex->setup(m_id);
		m_fragment->setup(m_id);
		//link
		GLint status;
		GLchar log[1024];
		glLinkProgram(m_id);
		glGetProgramiv(m_id, GL_LINK_STATUS, &status);
		if(status == 0)
		{
			glGetProgramInfoLog(m_id, sizeof(log), nullptr, log);
			fprintf(stderr, "Error linking shader program: %s\n", log);
			exit(EXIT_FAILURE);
		}
		//validate
		glValidateProgram(m_id);
		glGetProgramiv(m_id, GL_VALIDATE_STATUS, &status);
		if(status == 0)
		{
			glGetProgramInfoLog(m_id, sizeof(log), nullptr, log);
			fprintf(stderr, "Error validating shader program: %s\n", log);
			exit(EXIT_FAILURE);
		}
		//use
		glUseProgram(m_id);
	}
}