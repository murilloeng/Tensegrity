//ext
#include "../external/cpp/inc/GL/glew.h"

//tensegrity
#include "inc/GPU/VAO.h"

namespace gpu
{
	//constructors
	VAO::VAO(void) : m_id(0)
	{
		return;
	}

	//destructor
	VAO::~VAO(void)
	{
		if(glIsVertexArray(m_id)) glDeleteVertexArrays(1, &m_id);
	}

	//setup
	void VAO::bind(void)
	{
		glBindVertexArray(m_id);
	}
	void VAO::setup(void)
	{
		if(glIsVertexArray(m_id))
		{
			glDeleteVertexArrays(1, &m_id);
		}
		glGenVertexArrays(1, &m_id);
	}
	void VAO::unbind(void)
	{
		glBindVertexArray(0);
	}
}