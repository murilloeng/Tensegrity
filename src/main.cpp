//std
#include <omp.h>
#include <cmath>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <algorithm>

//math
#include "Math/inc/misc/misc.hpp"
#include "Math/inc/misc/bits.hpp"

//Tensegrity
#include "Tensegrity/inc/Solver.hpp"
#include "Tensegrity/inc/Tensegrity.hpp"

static void setup(Tensegrity& tensegrity)
{
	//tensegrity.m_nc = 3;		//number of cables			3, 4, 5, 6
	//tensegrity.m_Ht = 3.20e-01; //module height				0.32 -> (0.20, 0.44)
	//tensegrity.m_Hc = 1.40e-01;	//central cable height		0.14 -> (0.08, 0.20)
	//tensegrity.m_Rr = 1.40e-01;	//plate radius				0.14 -> (0.08, 0.20)
	//tensegrity.m_Ec = 2.00e+11;	//cables elastic modulus
	//tensegrity.m_dc = 1.50e-03;	//cables diameter			0.0015 -> (0.001, 0.002)
	//tensegrity.m_sr = 0.00e+00;	//pre-tension
	//tensegrity.m_q0 = 0.00e+00; //twist angle
}

int main(int argc, char** argv)
{
	//return
	return EXIT_SUCCESS;
}