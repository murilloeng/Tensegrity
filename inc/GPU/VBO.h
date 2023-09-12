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
	class VBO
	{
	public:
		//constructors
		VBO(void);

		//destructor
		virtual ~VBO(void);

		//draw
		void bind(void);
		void setup(void);
		void enable(void);
		void unbind(void);
		void disable(void);
		void allocate(void);
		void allocate(unsigned);
		void transfer(const float*) const;
		void transfer(const float*, unsigned, unsigned) const;

		//data
		GLuint m_id;
		unsigned m_vertices;
		std::vector<unsigned> m_sizes;
	};
}