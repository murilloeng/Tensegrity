#pragma once

//os
#ifdef _WIN32
#include <Windows.h>
#endif

//ext
#include <GL/gl.h>

namespace gpu
{
	class IBO
	{
	public:
		//constructors
		IBO(void);

		//destructor
		virtual ~IBO(void);

		//draw
		void bind(void);
		void setup(void);
		void unbind(void);
		void allocate(void);
		void allocate(unsigned);
		void transfer(const unsigned*) const;
		void transfer(const unsigned*, unsigned, unsigned) const;

		//data
		GLuint m_id;
		unsigned m_size;
	};
}