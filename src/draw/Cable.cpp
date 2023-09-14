//draw
#include "inc/draw/Cable.hpp"
#include "inc/draw/Canvas.hpp"

namespace draw
{
	//constructor
	Cable::Cable(void)
	{
		return;
	}

	//destructor
	Cable::~Cable(void)
	{
		return;
	}

	//draw
	void Cable::draw(void) const
	{
		const float dx[] = {0.5, 0, 0};
		const float vbo_data[] = {
			-0.9, -0.9, 0, 0, 0, 1,
			+0.9, -0.9, 0, 0, 0, 1,
			+0.9, +0.9, 0, 0, 0, 1
		};
		const unsigned ibo_data[] = {
			m_index_vertices + 0, m_index_vertices + 1, m_index_vertices + 2
		};
		memcpy(m_canvas->m_vbo_data + m_index_vertices, vbo_data, 18 * sizeof(float));
		memcpy(m_canvas->m_ibo_data + m_index_triangles, ibo_data, 3 * sizeof(unsigned));
	}
	unsigned Cable::lines(void) const
	{
		return 0;
	}
	unsigned Cable::points(void) const
	{
		return 0;
	}
	unsigned Cable::vertices(void) const
	{
		return 3;
	}
	unsigned Cable::triangles(void) const
	{
		return 1;
	}
}