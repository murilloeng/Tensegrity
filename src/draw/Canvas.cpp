//math
#include "inc/math/math.hpp"

//draw
#include "inc/draw/Canvas.hpp"
#include "inc/draw/Object.hpp"

namespace draw
{
	//constructor
	Canvas::Canvas(void) : 
		m_lines(0), m_points(0), m_vertices(0), m_triangles(0), m_vbo_data(nullptr), m_ibo_data(nullptr)
	{
		Object::m_canvas = this;
	}

	//destructror
	Canvas::~Canvas(void)
	{
		for(const Object* object : m_objects)
		{
			delete object;
		}
	}

	//draw
	void Canvas::draw(void)
	{
		//draw
		for(const Object* object : m_objects)
		{
			object->draw();
		}
		//setup
		m_box[0] = m_box[1] = m_box[2] = +FLT_MAX;
		m_box[3] = m_box[4] = m_box[5] = -FLT_MAX;
		//bounding box
		for(unsigned i = 0; i < m_vertices; i++)
		{
			for(unsigned j = 0; j < 3; j++)
			{
				m_box[j + 0] = fminf(m_box[j + 0], m_vbo_data[6 * i + j]);
				m_box[j + 3] = fmaxf(m_box[j + 3], m_vbo_data[6 * i + j]);
			}
		}
	}
	void Canvas::setup(void)
	{
		//setup
		m_lines = 0;
		m_points = 0;
		m_vertices = 0;
		m_triangles = 0;
		delete[] m_vbo_data;
		delete[] m_ibo_data;
		//objects
		for(Object* object : m_objects)
		{
			//index
			object->m_index_points = m_points;
			object->m_index_lines = 2 * m_lines;
			object->m_index_vertices = 6 * m_vertices;
			object->m_index_triangles = 3 * m_triangles;
			//count
			m_lines += object->lines();
			m_points += object->points();
			m_vertices += object->vertices();
			m_triangles += object->triangles();
		}
		//allocate
		m_vbo_data = new float[6 * m_vertices];
		m_ibo_data = new unsigned[m_points + 2 * m_lines + 3 * m_triangles];
		//indices
		for(Object* object : m_objects)
		{
			object->m_index_lines += m_points;
			object->m_index_triangles += m_points + 2 * m_lines;
		}
	}

	//affine
	void Canvas::move(unsigned start, unsigned count, const float* dx)
	{
		for(unsigned i = start; i < start + count; i++)
		{
			for(unsigned j = 0; j < 3; j++)
			{
				m_vbo_data[6 * i + j] += dx[j];
			}
		}
	}
	void Canvas::rotate(unsigned start, unsigned count, const float* xc, const float* tr)
	{
		for(unsigned i = start; i < start + count; i++)
		{
			math::rotate(m_vbo_data + 6 * i, xc, tr);
		}
	}
}