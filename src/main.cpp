//std
#include <cstdlib>

//Tensegrity
#include "inc/Tensegrity.hpp"

void translation_3_1(void)
{
	//data
	Tensegrity tensegrity;
	//setup
	tensegrity.m_ak.push_back(math::vec3(0, 0, 0.5));
	tensegrity.m_pk.push_back([] (double) { return math::vec3(0, 0, 1); });
	//solve
	tensegrity.solve();
}

int main(int argc, char** argv)
{
	//test
	translation_3_1();
	//return
	return EXIT_SUCCESS;
}