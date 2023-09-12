#pragma once

//os
#ifdef _WIN32
#include <Windows.h>
#endif

//std
#include <string>

//ext
#include <GL/gl.h>

namespace gpu
{
	class Shader
	{
	public:
		//constructors
		Shader(void);

		//destructor
		~Shader(void);

		//setup
		void setup(GLuint);

		//misc
		bool load_file(void);

		//data
		GLuint m_id;
		GLuint m_type;
		std::string m_path;
		std::string m_source;
	};
}