//draw
#include "inc/draw/Object.hpp"

namespace draw
{
	class Cable : public Object
	{
	public:
		//constructor
		Cable(void);

		//destructor
		~Cable(void);

		//draw
		void draw(void) const override;
		unsigned lines(void) const override;
		unsigned points(void) const override;
		unsigned vertices(void) const override;
		unsigned triangles(void) const override;
	};
}