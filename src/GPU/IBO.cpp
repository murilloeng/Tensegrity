//ext
#include "../external/cpp/inc/GL/glew.h"

//tensegrity
#include "inc/GPU/IBO.hpp"

namespace gpu
{
	//constructors
	IBO::IBO(void) : m_id(0), m_size(0)
	{
		return;
	}

	//destructor
	IBO::~IBO(void)
	{
		if(glIsBuffer(m_id)) glDeleteBuffers(1, &m_id);
	}

	//setup
	void IBO::bind(void)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
	}
	void IBO::setup(void)
	{
		if(glIsBuffer(m_id))
		{
			glDeleteBuffers(1, &m_id);
		}
		glGenBuffers(1, &m_id);
	}
	void IBO::unbind(void)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	void IBO::allocate(void)
	{
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_size * sizeof(unsigned), nullptr, GL_STATIC_DRAW);
	}
	void IBO::allocate(unsigned size)
	{
		m_size = size;
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_size * sizeof(unsigned), nullptr, GL_STATIC_DRAW);
	}
	void IBO::transfer(const unsigned* data) const
	{
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_size * sizeof(unsigned), data);
	}
	void IBO::transfer(const unsigned* data, unsigned offset, unsigned size) const
	{
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset * sizeof(unsigned), size * sizeof(unsigned), data);
	}
}