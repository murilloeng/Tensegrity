#pragma once

//os
#ifdef _WIN32
#include <Windows.h>
#endif

//std
#include <string>

//ext
#include <gl/GL.h>

namespace gpu
{
	class Texture
	{
	public:
		//constructors
		Texture(void);
		Texture(const char*);

		//destructor
		~Texture(void);

		//setup
		void setup(void);

		//load
		void load(void);

		//bind
		void bind(GLenum);
		void unbind(void);

		//data
		GLuint m_id;
		std::string m_path;
	};
}