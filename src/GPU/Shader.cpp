//os
#ifdef _WIN32
#include <Windows.h>
#endif

//ext
#include "../external/cpp/inc/GL/glew.h"

//tensegrity
#include "inc/GPU/Shader.h"

namespace gpu
{
	//constructors
	Shader::Shader(void)
	{
		return;
	}

	//destructor
	Shader::~Shader(void)
	{
		return;
	}

	//setup
	void Shader::setup(GLuint program)
	{
		//source
		if(!load_file())
		{
			fprintf(stderr, "\tError: Unable to load shader source! (%d)\n", m_type);
			exit(EXIT_FAILURE);
		}
		//create
		m_id = glCreateShader(m_type);
		if(m_id == 0)
		{
			fprintf(stderr, "\tError: Unable to create shader! (%d)\n", m_type);
			exit(EXIT_FAILURE);
		}
		//source
		const GLchar* p = m_source.c_str();
		glShaderSource(m_id, 1, &p, nullptr);
		//compile
		GLint status;
		GLchar log[1024];
		glCompileShader(m_id);
		glGetShaderiv(m_id, GL_COMPILE_STATUS, &status);
		if(status == 0)
		{
			glGetShaderInfoLog(m_id, sizeof(log), nullptr, log);
			fprintf(stderr, "Error compiling shader (%d): %s\n", m_type, log);
			exit(EXIT_FAILURE);
		}
		//attach
		glAttachShader(program, m_id);
	}

	//misc
	bool Shader::load_file(void)
	{
		//open
		FILE* file = fopen(m_path.c_str(), "r");
		//check
		if(!file) return false;
		//read
		m_source.clear();
		while(!feof(file)) m_source += fgetc(file);
		m_source.back() = '\0';
		//close
		fclose(file);
		//return
		return true;
	}
}