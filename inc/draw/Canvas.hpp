#pragma once

//std
#include <vector>

namespace draw
{
	class Object;
}

namespace draw
{
	class Canvas
	{
	public:
		//constructor
		Canvas(void);

		//destructor
		~Canvas(void);

		//draw
		void draw(void);
		void setup(void);

		//affine
		void move(unsigned, unsigned, const float*);
		void rotate(unsigned, unsigned, const float*, const float*);

		//data
		unsigned m_lines;
		unsigned m_points;
		unsigned m_vertices;
		unsigned m_triangles;

		float m_box[6];
		float* m_vbo_data;
		unsigned* m_ibo_data;

		std::vector<Object*> m_objects;
	};
}