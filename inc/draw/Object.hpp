#pragma once

namespace gpu
{
	class IBO;
	class VBO;
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

		//data

	};
}