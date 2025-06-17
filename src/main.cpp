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
	map(0, 3, 1e2);
	map(0, 3, 1e3);
	map(0, 4, 1e2);
	map(0, 4, 1e3);
	map(0, 5, 1e2);
	map(0, 5, 1e3);
	map(0, 6, 1e2);
	map(0, 6, 1e3);
	// load_nonlinear_vertical_radial_peak(7);
	// load_nonlinear_vertical_radial_peak(8);
	// load_nonlinear_vertical_radial_peak(9);
	// load_nonlinear_vertical_radial_peak(10);
	//return
	return EXIT_SUCCESS;
}