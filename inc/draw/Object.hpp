#pragma once

namespace draw
{
	class Canvas;
}


namespace draw
{
	class Object
	{
	public:
		//constructor
		Object(void);

		//destructor
		virtual ~Object(void);

		//draw
		virtual void draw(void) const = 0;
		virtual unsigned lines(void) const = 0;
		virtual unsigned points(void) const = 0;
		virtual unsigned vertices(void) const = 0;
		virtual unsigned triangles(void) const = 0;

		//affine
		void move(const float*) const;
		void rotate(const float*, const float*) const;

		//data
		static Canvas* m_canvas;

		unsigned m_index_lines;
		unsigned m_index_points;
		unsigned m_index_vertices;
		unsigned m_index_triangles;
	};
}