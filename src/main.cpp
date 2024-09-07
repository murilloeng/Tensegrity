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

void force(void)
{
	//data
	const uint32_t nc = 6;
	const uint32_t nr = 100;
	const uint32_t na = 100;
	const uint32_t nt = omp_get_max_threads();
	Tensegrity* tensegrity = new Tensegrity[nt];
	//data
	const double Pr = 1.00e+03;
	const double fr = 2.00e+01;
	const double Ht = tensegrity[0].height_total();
	const double dp = tensegrity[0].plate_diameter();
	const double dc = tensegrity[0].cables_diameter();
	//setup
	const double Ac = M_PI * dc * dc / 4;
	for(uint32_t i = 0; i < nt; i++)
	{
		tensegrity[i].cables(nc);
		tensegrity[i].residual_stress(fr / Ac);
		tensegrity[i].add_load({0, 0, -Pr}, {});
	}
	//solve
	using namespace std::chrono;
	double* state = new double[8 * nr * na];
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	for(int32_t i = 0; i < nr; i++)
	{
		bool test = true;
		#pragma omp parallel for
		for(int32_t j = 0; j < na; j++)
		{
			//data
			const double t = 2 * M_PI * j / na;
			const double r = dp / 2 * (i + 1) / nr;
			const int32_t ti = omp_get_thread_num();
			//load
			tensegrity[ti].load_position(0, {r * cos(t), r * sin(t), Ht});
			//solve
			tensegrity[ti].solver()->solve();
			printf("radius: %02d angle: %02d\n", i, j);
			const double* ts = tensegrity[ti].solver()->state();
			const math::vec3 vt = math::quat(ts + 3).pseudo();
			if(!tensegrity[ti].solver()->equilibrium()) test = false;
			state[8 * (na * i + j) + 0] = r;
			state[8 * (na * i + j) + 1] = t;
			state[8 * (na * i + j) + 2] = ts[0];
			state[8 * (na * i + j) + 3] = ts[1];
			state[8 * (na * i + j) + 4] = ts[2];
			state[8 * (na * i + j) + 5] = vt[0];
			state[8 * (na * i + j) + 6] = vt[1];
			state[8 * (na * i + j) + 7] = vt[2];
		}
		if(!test) break;
	}
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	const double td = (double) duration_cast<milliseconds>(t2 - t1).count() / 1000;
	printf("Solution time: %.2lf s\n", td);
	//flush
	FILE* file = fopen("data/force.txt", "w");
	for(uint32_t i = 0; i < nr; i++)
	{
		for(uint32_t j = 0; j <= na; j++)
		{
			for(uint32_t k = 0; k < 8; k++)
			{
				fprintf(file, "%+.6e ", state[8 * (na * i + j % na) + k]);
			}
			fprintf(file, "\n");
		}
		fprintf(file, "\n");
	}
	fclose(file);
	delete[] state;
	delete[] tensegrity;
}

int main(void)
{
	//test
	force();
	//return
	return EXIT_SUCCESS;
}