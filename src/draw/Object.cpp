//draw
#include "inc/draw/Object.hpp"
#include "inc/draw/Canvas.hpp"

namespace draw
{
	//constructor
	Object::Object(void) : m_index_lines(0), m_index_points(0), m_index_vertices(0), m_index_triangles(0)
	{
		return;
	}

	//destructor
	Object::~Object(void)
	{
		return;
	}

	//affine
	void Object::move(const float* dx) const
	{
		m_canvas->move(m_index_vertices, vertices(), dx);
	}
	void Object::rotate(const float* xr, const float* tr) const
	{
		m_canvas->rotate(m_index_vertices, vertices(), xr, tr);
	}

	//data
	Canvas* Object::m_canvas = nullptr;
}