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
	// //test
	// Tensegrity tensegrity;
	// const double fr = 1.00e+02;
	// const double Pr = 1.00e+03;
	// const double Ht = tensegrity.height_total();
	// const double dp = tensegrity.plate_diameter();
	// //setup
	// tensegrity.tension(fr);
	// tensegrity.solver()->step_max(10000);
	// tensegrity.add_load({Pr, 0, 0}, {dp / 2, 0, Ht});
	// tensegrity.solver()->strategy(strategy_type::arc_length);
	// tensegrity.solver()->stop_condition(uint32_t(stop_condition::load_negative));
	// //solve
	// tensegrity.solver()->solve();

	//test
	load_nonlinear_horizontal_radial(4, false);
	load_nonlinear_horizontal_radial(5, false);
	load_nonlinear_horizontal_radial(6, false);
	//return
	return EXIT_SUCCESS;
}