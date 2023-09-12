#pragma once

//os
#ifdef _WIN32
#include <Windows.h>
#endif

//ext
#include <GL/gl.h>

namespace gpu
{
	class Shader;

	class Program
	{
	public:
		//constructors
		Program(void);

		//destructor
		~Program(void);

		//setup
		void use(void);
		void setup(void);

		//data
		GLuint m_id;
		Shader* m_vertex;
		Shader* m_fragment;
	};
}