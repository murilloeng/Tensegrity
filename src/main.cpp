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
#include "Tensegrity/inc/Solver.hpp"
#include "Tensegrity/inc/Strategy.hpp"
#include "Tensegrity/inc/Tensegrity.hpp"

void fun_energy(double* U, const double* d, void** args)
{
	//data
	double sd[7];
	math::vec3(sd + 0) = math::vec3(d + 0);
	math::quat(sd + 3) = math::vec3(d + 3).quaternion();
	((Tensegrity*) args[0])->solver()->state(sd);
	//energy
	U[0] = ((Tensegrity*) args[0])->internal_energy();
}
void fun_force(double* f, const double* d, void** args)
{
	//data
	double sd[7];
	math::vector fm(f, 6);
	math::vec3(sd + 0) = math::vec3(d + 0);
	math::quat(sd + 3) = math::vec3(d + 3).quaternion();
	((Tensegrity*) args[0])->solver()->state(sd);
	//force
	((Tensegrity*) args[0])->internal_force(fm);
}
void fun_stiffness(double* K, const double* d, void** args)
{
	//data
	double sd[7];
	math::matrix Km(K, 6, 6);
	math::vec3(sd + 0) = math::vec3(d + 0);
	math::quat(sd + 3) = math::vec3(d + 3).quaternion();
	((Tensegrity*) args[0])->solver()->state(sd);
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
	tensegrity.strain_measure(Strain::strain_measure::logarithmic);
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
	tensegrity.strain_measure(Strain::strain_measure::logarithmic);
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

void nonlinear_vertical_radial(uint32_t nc)
{
	//data
	char label[200];
	Tensegrity tensegrity;
	const uint32_t nr = 10;
	const double fr = 1.00e+02;
	const double Pr = 1.00e+03;
	const double dl = 4.00e-02;
	const double Ht = tensegrity.height_center();
	const double dp = tensegrity.plate_diameter();
	//setup
	tensegrity.cables(nc);
	tensegrity.tension(fr);
	tensegrity.solver()->log(false);
	tensegrity.solver()->watch_dof(2);
	tensegrity.solver()->step_max(200000);
	tensegrity.solver()->load_increment(dl);
	tensegrity.add_load({0, 0, -Pr}, {dp / 2, 0, Ht});
	tensegrity.solver()->strategy(strategy_type::arc_length);
	//solve
	for(uint32_t i = 0; i < nr; i++)
	{
		//print
		printf("position: %d\n", i);
		sprintf(label, "vertical/radial/%d/model-%d", tensegrity.cables(), i);
		//setup
		tensegrity.label(label);
		tensegrity.load_position(0, {dp / 2 * (i + 1) / nr, 0, Ht});
		//solve
		tensegrity.solver()->solve();
	}
}
void nonlinear_vertical_angular(uint32_t nc)
{
	//data
	char label[200];
	Tensegrity tensegrity;
	const uint32_t na = 10;
	const double fr = 1.00e+02;
	const double Pr = 1.00e+03;
	const double dl = 4.00e-02;
	const double Ht = tensegrity.height_center();
	const double dp = tensegrity.plate_diameter();
	//setup
	tensegrity.cables(nc);
	tensegrity.tension(fr);
	tensegrity.solver()->log(false);
	tensegrity.solver()->watch_dof(2);
	tensegrity.solver()->step_max(200000);
	tensegrity.solver()->load_increment(dl);
	tensegrity.add_load({0, 0, -Pr}, {dp / 2, 0, Ht});
	tensegrity.solver()->strategy(strategy_type::arc_length);
	//solve
	for(uint32_t i = 0; i <= na; i++)
	{
		//print
		printf("angle: %d\n", i);
		sprintf(label, "vertical/angular/%d/model-%d", tensegrity.cables(), i);
		//setup
		tensegrity.label(label);
		const double t = M_PI / 3 * i / na;
		tensegrity.load_position(0, {dp / 2 * cos(t), dp / 2 * sin(t), Ht});
		//solve
		tensegrity.solver()->solve();
	}
}

int main(int32_t argc, char* argv[])
{
	//test
	nonlinear_vertical_angular(3);
	nonlinear_vertical_angular(4);
	nonlinear_vertical_angular(5);
	nonlinear_vertical_angular(6);
	//return
	return EXIT_SUCCESS;
}