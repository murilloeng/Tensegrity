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

		//setup
		void setup(void);

		//data
		unsigned m_lines;
		unsigned m_points;
		unsigned m_triangles;

		float* m_vbo_data;
		unsigned* m_ibo_data;

		std::vector<Object*> m_objects;
	};
}