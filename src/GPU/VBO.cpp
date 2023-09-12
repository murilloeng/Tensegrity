//ext
#include "../external/cpp/inc/GL/glew.h"

//tensegrity
#include "inc/GPU/VBO.hpp"

namespace gpu
{
	//constructors
	VBO::VBO(void) : m_id(0), m_vertices(0)
	{
		return;
	}

	//destructor
	VBO::~VBO(void)
	{
		if(glIsBuffer(m_id)) glDeleteBuffers(1, &m_id);
	}

	//setup
	void VBO::bind(void)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_id);
	}
	void VBO::setup(void)
	{
		if(glIsBuffer(m_id))
		{
			glDeleteBuffers(1, &m_id);
		}
		glGenBuffers(1, &m_id);
	}
	void VBO::enable(void)
	{
		unsigned c = 0, n = 0;
		for(unsigned s : m_sizes) n += s;
		for(unsigned i = 0; i < m_sizes.size(); i++)
		{
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, m_sizes[i], GL_FLOAT, GL_FALSE, n * sizeof(float), (unsigned*) (c * sizeof(float)));
			c += m_sizes[i];
		}
	}
	void VBO::unbind(void)
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void VBO::disable(void)
	{
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}
	void VBO::allocate(void)
	{
		unsigned n = 0;
		for(unsigned s : m_sizes) n += s;
		glBufferData(GL_ARRAY_BUFFER, n * m_vertices * sizeof(float), nullptr, GL_STATIC_DRAW);
	}
	void VBO::allocate(unsigned vertices)
	{
		unsigned n = 0;
		m_vertices = vertices;
		for(unsigned s : m_sizes) n += s;
		glBufferData(GL_ARRAY_BUFFER, n * m_vertices * sizeof(float), nullptr, GL_STATIC_DRAW);
	}
	void VBO::transfer(const float* data) const
	{
		unsigned n = 0;
		for(unsigned s : m_sizes) n += s;
		glBufferSubData(GL_ARRAY_BUFFER, 0, n * m_vertices * sizeof(float), data);
	}
	void VBO::transfer(const float* data, unsigned offset, unsigned size) const
	{
		unsigned n = 0;
		for(unsigned s : m_sizes) n += s;
		glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(float), size * sizeof(float), data);
	}
}