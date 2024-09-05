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

void fun_energy(double* U, const double* d, void** args)
{
	//data
	double* sd = ((Tensegrity*) args[0])->solver()->m_state_new;
	//state
	math::vector(sd + 0, 3) = d;
	math::vector(sd + 3, 4) = math::vec3(d + 3).quaternion();
	//energy
	U[0] = ((Tensegrity*) args[0])->internal_energy();
}
void fun_force(double* f, const double* d, void** args)
{
	//data
	math::vector fm(f, 6);
	double* sd = ((Tensegrity*) args[0])->solver()->m_state_new;
	//state
	math::vector(sd + 0, 3) = d;
	math::vector(sd + 3, 4) = math::vec3(d + 3).quaternion();
	//force
	((Tensegrity*) args[0])->internal_force(fm);
}
void fun_stiffness(double* K, const double* d, void** args)
{
	//data
	math::matrix Km(K, 6, 6);
	double* sd = ((Tensegrity*) args[0])->solver()->m_state_new;
	//state
	math::vector(sd + 0, 3) = d;
	math::vector(sd + 3, 4) = math::vec3(d + 3).quaternion();
	//stiffness
	((Tensegrity*) args[0])->stiffness(Km);
}

void test_force(void)
{
	//data
	Tensegrity tensegrity;
	const uint32_t nt = 10000;
	math::vector d(6), fa(6), fn(6);
	//setup
	void* args[] = {&tensegrity};
	tensegrity.elastic_modulus(2.00e+11);
	tensegrity.residual_stress(0.00e+00);
	//test
	srand(uint32_t(time(nullptr)));
	for(uint32_t i = 0; i < nt; i++)
	{
		d.randu();
		fun_force(fa.data(), d.data(), args);
		math::ndiff(fun_energy, fn.data(), d.data(), args, 1, 6, 1e-5);
		if((fa - fn).norm() < 1e-5 * fa.norm())
		{
			printf("%d: ok!\n", i);
		}
		else
		{
			printf("%d: not ok!\n", i);
			(fa - fn).print("fe", 1e-5);
			break;
		}
	}
}
void test_stiffness(void)
{
	//data
	math::vector d(6);
	Tensegrity tensegrity;
	const uint32_t nt = 10000;
	math::matrix Ka(6, 6), Kn(6, 6);
	//setup
	void* args[] = {&tensegrity};
	tensegrity.elastic_modulus(2.00e+11);
	tensegrity.residual_stress(0.00e+00);
	//test
	srand(uint32_t(time(nullptr)));
	for(uint32_t i = 0; i < nt; i++)
	{
		d.randu();
		fun_stiffness(Ka.data(), d.data(), args);
		math::ndiff(fun_force, Kn.data(), d.data(), args, 6, 6, 1e-5);
		if((Ka - Kn).norm() < 1e-5 * Ka.norm())
		{
			printf("%d: ok!\n", i);
		}
		else
		{
			printf("%d: not ok!\n", i);
			(Ka - Kn).print("Ke", 1e-5 * Ka.norm());
			break;
		}
	}
}

int main(void)
{
	//test
	test_stiffness();
	//return
	return EXIT_SUCCESS;
}