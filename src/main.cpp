//std
#include <omp.h>
#include <cmath>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <algorithm>

//math
#include "Math/inc/misc/misc.hpp"

//Tensegrity
#include "Tensegrity/inc/Map.hpp"
#include "Tensegrity/inc/tests.hpp"
#include "Tensegrity/inc/Solver.hpp"
#include "Tensegrity/inc/Strategy.hpp"
#include "Tensegrity/inc/Tensegrity.hpp"

int main(int32_t argc, char* argv[])
{
	//test
	map(2, 3, 1.00e+02);
	map(2, 4, 1.00e+02);
	map(2, 5, 1.00e+02);
	map(2, 6, 1.00e+02);
	map(2, 3, 1.00e+03);
	map(2, 4, 1.00e+03);
	map(2, 5, 1.00e+03);
	map(2, 6, 1.00e+03);
	//return
	return EXIT_SUCCESS;
}