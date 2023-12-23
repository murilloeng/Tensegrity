//std
#include <cstdlib>

//Tensegrity
#include "inc/Tensegrity.hpp"

//canvas
#include "Canvas/lib/inc/Managers/Glut.hpp"

int main(int argc, char** argv)
{
	//data
	Tensegrity tensegrity;
	canvas::Glut app(argc, argv, "../Canvas/lib/shd/");
	//scene
	tensegrity.draw_model(app.scene());
	app.scene()->update(true);
	//start
	app.start();
	//return
	return EXIT_SUCCESS;
}