#pragma once

//os
#ifdef _WIN32
#include <Windows.h>
#endif

//std
#include <vector>

//ext
#include <GL/gl.h>

namespace gpu
{
	class VAO
	{
	public:
		//constructors
		VAO(void);

		//destructor
		virtual ~VAO(void);

		//draw
		void bind(void);
		void setup(void);
		void unbind(void);

		//data
		GLuint m_id;
	};
}