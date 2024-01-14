//std
#include <cstdlib>

//Tensegrity
#include "inc/Solver.hpp"
#include "inc/Tensegrity.hpp"

void translation_3_1(void)
{
	//data
	Tensegrity tensegrity;
	const double P = 1.00e+00;
	//setup
	tensegrity.m_solver->m_dl = 1.00e-3;
	tensegrity.m_ak.push_back(math::vec3(0, 0, 0.5));
	tensegrity.m_pk.push_back([P] (double) { return math::vec3(0, 0, P); });
	//solve
	tensegrity.m_solver->solve();
}

int main(int argc, char** argv)
{
	//test
	translation_3_1();
	//return
	return EXIT_SUCCESS;
}