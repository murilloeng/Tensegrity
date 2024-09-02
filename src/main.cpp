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

//qt
#include <QtWidgets/QApplication>

//Tensegrity
#include "Tensegrity/inc/Solver.hpp"
#include "Tensegrity/inc/Window.hpp"
#include "Tensegrity/inc/Tensegrity.hpp"

extern "C"
{
	void dgesdd_(const char*, const int*, const int*, double*, const int*, double*, double*, const int*, double*, const int*, double*, int*, int*, int*);
}

static void setup(Tensegrity& tensegrity)
{
	tensegrity.m_nc = 3;		//number of cables			3, 4, 5, 6
	tensegrity.m_Ht = 3.20e-01; //module height				0.32 -> (0.20, 0.44)
	tensegrity.m_Hc = 1.40e-01;	//central cable height		0.14 -> (0.08, 0.20)
	tensegrity.m_Rr = 1.40e-01;	//plate radius				0.14 -> (0.08, 0.20)
	tensegrity.m_Ec = 2.00e+11;	//cables elastic modulus
	tensegrity.m_dc = 1.50e-03;	//cables diameter			0.0015 -> (0.001, 0.002)
	tensegrity.m_sr = 0.00e+00;	//pre-tension
	tensegrity.m_q0 = 0.00e+00; //twist angle
}
static void window(int argc, char** argv)
{
	//application
	QApplication application(argc, argv);
	//window
	Window window;
	window.show();
	//start
	application.exec();
}
static void dof_energy(void)
{
	//data
	Tensegrity tensegrity;
	const unsigned ns = 1000;
	const double ul = 1.00e-01;
	const double tl = M_PI / 3;
	FILE* file = fopen("data/energy.txt", "w");
	//energy
	setup(tensegrity);
	for(unsigned i = 0; i <= ns; i++)
	{
		const double u = 2 * ul * i / ns - ul;
		const double t = 2 * tl * i / ns - tl;
		for(unsigned j = 0; j < 3; j++)
		{
			tensegrity.m_solver->clear_state();
			tensegrity.m_solver->m_state_new[j + 0] = u;
			fprintf(file, "%+.6e %+.6e ", u, tensegrity.internal_energy());
		}
		for(unsigned j = 0; j < 3; j++)
		{
			tensegrity.m_solver->clear_state();
			tensegrity.m_solver->m_state_new[3] = cos(t / 2);
			tensegrity.m_solver->m_state_new[j + 4] = sin(t / 2);
			fprintf(file, "%+.6e %+.6e ", t, tensegrity.internal_energy());
		}
		fprintf(file, "\n");
	}
	//close
	fclose(file);
}
static void dof_internal_force(void)
{
	//data
	math::vector fi(6);
	Tensegrity tensegrity;
	const unsigned ns = 1000;
	const double ul = 1.00e-01;
	const double tl = M_PI / 3;
	FILE* file = fopen("data/force.txt", "w");
	//energy
	setup(tensegrity);
	for(unsigned i = 0; i <= ns; i++)
	{
		const double u = 2 * ul * i / ns - ul;
		const double t = 2 * tl * i / ns - tl;
		for(unsigned j = 0; j < 3; j++)
		{
			tensegrity.m_solver->clear_state();
			tensegrity.m_solver->m_state_new[j + 0] = u;
			tensegrity.internal_force(fi);
			fprintf(file, "%+.6e ", u);
			for(unsigned k = 0; k < 6; k++) fprintf(file, "%+.6e ", fi[k]);
		}
		for(unsigned j = 0; j < 3; j++)
		{
			tensegrity.m_solver->clear_state();
			tensegrity.m_solver->m_state_new[3] = cos(t / 2);
			tensegrity.m_solver->m_state_new[j + 4] = sin(t / 2);
			tensegrity.internal_force(fi);
			fprintf(file, "%+.6e ", t);
			for(unsigned k = 0; k < 6; k++) fprintf(file, "%+.6e ", fi[k]);
		}
		fprintf(file, "\n");
	}
	//close
	fclose(file);
}
static void fun_stiffness(double* K, const double* d, void** args)
{
	//data
	math::matrix Km(K, 6, 6);
	const math::vec3 x(d + 0), t(d + 3);
	Tensegrity* tensegrity = (Tensegrity*) args[0];
	const math::quat q(tensegrity->m_solver->m_state_old + 3);
	//state
	math::vec3(tensegrity->m_solver->m_state_new + 0) = x;
	math::quat(tensegrity->m_solver->m_state_new + 3) = q * t.quaternion();
	//force
	tensegrity->stiffness(Km);
}
static void fun_internal_force(double* f, const double* d, void** args)
{
	//data
	math::vector fm(f, 6);
	const math::vec3 x(d + 0), t(d + 3);
	Tensegrity* tensegrity = (Tensegrity*) args[0];
	const math::quat q(tensegrity->m_solver->m_state_old + 3);
	//state
	math::vec3(tensegrity->m_solver->m_state_new + 0) = x;
	math::quat(tensegrity->m_solver->m_state_new + 3) = q * t.quaternion();
	//force
	tensegrity->internal_force(fm);
}
static void fun_internal_energy(double* U, const double* d, void** args)
{
	//data
	const math::vec3 x(d + 0), t(d + 3);
	Tensegrity* tensegrity = (Tensegrity*) args[0];
	const math::quat q(tensegrity->m_solver->m_state_old + 3);
	//state
	math::vec3(tensegrity->m_solver->m_state_new + 0) = x;
	math::quat(tensegrity->m_solver->m_state_new + 3) = q * t.quaternion();
	//force
	U[0] = tensegrity->internal_energy();
}
static void test_force(void)
{
	//data
	math::quat q;
	Tensegrity tensegrity;
	const unsigned nt = 10000;
	void* args[] = { &tensegrity };
	math::vector d(6), fa(6), fn(6), fr(6);
	//test
	setup(tensegrity);
	srand((unsigned) time(nullptr));
	for(unsigned i = 0; i < nt; i++)
	{
		//setup
		d.randu();
		q.randu();
		const math::vec3 t(d.data() + 3);
		math::vector(q.data(), 4) = q.unit();
		math::quat(tensegrity.m_solver->m_state_old + 3) = q;
		//functions
		fun_internal_force(fa.data(), d.data(), args);
		math::ndiff(fun_internal_energy, fn.data(), d.data(), args, 1, 6, 1e-8);
		math::vec3(fa.data() + 3) *= (q.rotation() * t.rotation_gradient()).transpose();
		//residue
		fr = fa - fn;
		if(fr.norm() < 1e-5 * fmax(fa.norm(), 1))
		{
			printf("%04d %.2e ok!\n", i, fr.norm());
		}
		else
		{
			fr.print("fr", 1e-5 * fa.norm());
			break;
		}
	}
}
static void test_stiffness(void)
{
	//data
	math::quat q;
	math::vector d(6);
	Tensegrity tensegrity;
	const unsigned nt = 10000;
	void* args[] = { &tensegrity };
	math::matrix Ka(6, 6), Kn(6, 6), Kr(6, 6);
	//test
	setup(tensegrity);
	srand((unsigned) time(nullptr));
	for(unsigned i = 0; i < nt; i++)
	{
		//setup
		d.randu();
		q.randu();
		math::vector(q.data(), 4) = q.unit();
		math::quat(tensegrity.m_solver->m_state_old + 3) = q;
		//functions
		fun_stiffness(Ka.data(), d.data(), args);
		math::ndiff(fun_internal_force, Kn.data(), d.data(), args, 6, 6, 1e-8);
		//residue
		Kr = Ka - Kn;
		if(Kr.norm() < 1e-5 * fmax(Ka.norm(), 1))
		{
			printf("%04d %.2e ok!\n", i, Kr.norm());
		}
		else
		{
			Kr.print("Kr", 1e-5 * Ka.norm());
			break;
		}
	}
}

static void vertical_analytic(void)
{
	//data
	double t;
	math::vector d(3);
	math::vector f(3);
	math::matrix K(3, 3);
	const unsigned nc = 3;
	const double m = 1.00e+01;
	const double g = 9.81e+00;
	const double Ec = 2.00e+11;
	const double dc = 1.50e-03;
	const double Hc = 1.40e-01;
	const double Ht = 3.20e-01;
	const double Rr = 1.40e-01;
	const double a1 = 1.40e-01;
	const double a2 = 0.00e+00;
	const double Ac = M_PI * dc * dc / 4;
	char string[8 * sizeof(unsigned) + 1];
	//solve
	unsigned mask = 0;
	for(unsigned i = 0; i < (1 << nc); i++)
	{
		K.zeros();
		f(0) = -m * g;
		f(1) = -m * g * a2;
		f(2) = +m * g * a1;
		K(0, 0) = Ec * Ac / Hc;
		for(unsigned j = 0; j < nc; j++)
		{
			if((1 << j) & mask)
			{
				t = 2 * M_PI * j / nc;
				K(0, 0) += Ec * Ac / Ht;
				K(0, 1) += Ec * Ac / Ht * Rr * sin(t);
				K(0, 2) -= Ec * Ac / Ht * Rr * cos(t);
				K(1, 0) += Ec * Ac / Ht * Rr * sin(t);
				K(2, 0) -= Ec * Ac / Ht * Rr * cos(t);
				K(1, 1) += Ec * Ac / Ht * Rr * Rr * sin(t) * sin(t);
				K(1, 2) -= Ec * Ac / Ht * Rr * Rr * sin(t) * cos(t);
				K(2, 1) -= Ec * Ac / Ht * Rr * Rr * sin(t) * cos(t);
				K(2, 2) += Ec * Ac / Ht * Rr * Rr * cos(t) * cos(t);
			}
		}
		if(K.solve(d, f) && d[0] < 0)
		{
			bool test = true;
			for(unsigned j = 0; j < nc; j++)
			{
				if((1 << j) & mask)
				{
					t = 2 * M_PI * j / nc;
					test = test && (d[0] + Rr * sin(t) * d[1] - Rr * cos(t) * d[2] > 0);
				}
			}
			if(test)
			{
				math::binary_form(string, mask);
				printf("compatible mask found: %s\n", string);
				d.transpose().print("d");
			}
		}
		mask++;
	}
}

static void load_1(void)
{
	//data
	const int np = 100;
	const double m = 1.00e+01;
	const double g = 9.81e+00;
	Tensegrity tensegrity[16];
	using namespace std::chrono;
	const time_point<high_resolution_clock> t1 = high_resolution_clock::now();
	//setup
	omp_set_num_threads(16);
	for(unsigned i = 0; i < 16; i++)
	{
		setup(tensegrity[i]);
		tensegrity[i].m_pk.push_back({0, 0, -m * g});
		tensegrity[i].m_ak.push_back({0, 0, tensegrity[i].m_Ht});
	}
	double* state = new double[6 * (np + 1)];
	//computation
	#pragma omp parallel for
	for(int i = 0; i <= np; i++)
	{
		//data
		const unsigned ic = omp_get_thread_num();
		//setup
		tensegrity[ic].m_solver->clear_state();
		tensegrity[ic].m_pk[0][2] = m * g * i / np;
		//solve
		tensegrity[ic].m_solver->solve();
		if(!tensegrity[ic].m_solver->m_equilibrium) continue;
		//save
		memcpy(state + 6 * i, tensegrity[ic].m_solver->m_dx.data(), 6 * sizeof(double));
	}
	//save
	FILE* file = fopen("data/load_1.txt", "w");
	for(unsigned i = 0; i <= np; i++)
	{
		fprintf(file, "%+.6e ", m * g * i / np);
		for(unsigned k = 0; k < 6; k++) fprintf(file, "%+.6e ", state[6 * i + k]);
		fprintf(file, "\n");
	}
	fclose(file);
	//clock
	const time_point<high_resolution_clock> t2 = high_resolution_clock::now();
	printf("time: %.2lf s\n", double(duration_cast<milliseconds>(t2 - t1).count()) / 1e3);
	//delete
	delete[] state;
}
static void load_2(void)
{
	//data
	const int nr = 100;
	const int nt = 300;
	const double m = 1.00e+01;
	const double g = 9.81e+00;
	Tensegrity tensegrity[16];
	using namespace std::chrono;
	const time_point<high_resolution_clock> t1 = high_resolution_clock::now();
	//setup
	omp_set_num_threads(16);
	for(unsigned i = 0; i < 16; i++)
	{
		setup(tensegrity[i]);
		tensegrity[i].m_nc = 3;
		tensegrity[i].m_pk.push_back({0, 0, -m * g});
		tensegrity[i].m_ak.push_back({0, 0, tensegrity[i].m_Ht});
	}
	double* state = new double[6 * nr * nt];
	//computation
	#pragma omp parallel for
	for(int i = 0; i < nr * nt; i++)
	{
		//data
		const unsigned ir = i / nt;
		const unsigned it = i % nt;
		const unsigned ic = omp_get_thread_num();
		//data
		const double t = 2 * M_PI * it / nt;
		const double r = tensegrity[ic].m_Rr * (ir + 1) / nr;
		//setup
		tensegrity[ic].m_solver->clear_state();
		tensegrity[ic].m_ak[0][0] = r * cos(t);
		tensegrity[ic].m_ak[0][1] = r * sin(t);
		tensegrity[ic].m_ak[0][2] = tensegrity[ic].m_Ht;
		//solve
		tensegrity[ic].m_solver->solve();
		if(!tensegrity[ic].m_solver->m_equilibrium)
		{
			printf("Solver failed!\n");
			continue;
		}
		//save
		memcpy(state + 6 * i, tensegrity[ic].m_solver->m_dx.data(), 6 * sizeof(double));
	}
	//save
	FILE* file = fopen("data/load_2.txt", "w");
	for(unsigned i = 0; i < nr; i++)
	{
		for(unsigned j = 0; j <= nt; j++)
		{
			//data
			const double t = 2 * M_PI * j / nt;
			const double r = tensegrity[0].m_Rr * i / nr;
			//write
			fprintf(file, "%+.6e %+.6e ", r, t);
			for(unsigned k = 0; k < 6; k++) fprintf(file, "%+.6e ", state[6 * (nt * i + j % nt) + k]);
			fprintf(file, "\n");
		}
		fprintf(file, "\n");
	}
	fclose(file);
	//clock
	const time_point<high_resolution_clock> t2 = high_resolution_clock::now();
	printf("time: %.2lf s\n", double(duration_cast<milliseconds>(t2 - t1).count()) / 1e3);
	//delete
	delete[] state;
}
static void load_3(void)
{
	//data
	const int nr = 100;
	const int nt = 300;
	const int np = 100;
	const double m = 1.00e+01;
	const double g = 9.81e+00;
	Tensegrity tensegrity[16];
	using namespace std::chrono;
	const time_point<high_resolution_clock> t1 = high_resolution_clock::now();
	//setup
	omp_set_num_threads(16);
	for(unsigned i = 0; i < 16; i++)
	{
		setup(tensegrity[i]);
		tensegrity[i].m_pk.push_back({0, 0, -m * g});
		tensegrity[i].m_ak.push_back({0, 0, tensegrity[i].m_Ht});
	}
	double* state = new double[6 * nr * nt];
	//loop
	for(unsigned k = 0; k <= np; k++)
	{
		//computation
		#pragma omp parallel for
		for(int i = 0; i < nr * nt; i++)
		{
			//data
			const unsigned ir = i / nt;
			const unsigned it = i % nt;
			const unsigned ic = omp_get_thread_num();
			//data
			const double t = 2 * M_PI * it / nt;
			const double r = tensegrity[ic].m_Rr * (ir + 1) / nr;
			tensegrity[ic].m_Ht = 0.20 + (0.44 - 0.20) * k / np;
			//setup
			tensegrity[ic].m_solver->clear_state();
			tensegrity[ic].m_ak[0][0] = r * cos(t);
			tensegrity[ic].m_ak[0][1] = r * sin(t);
			tensegrity[ic].m_ak[0][2] = tensegrity[ic].m_Ht;
			//solve
			tensegrity[ic].m_solver->solve();
			if(!tensegrity[ic].m_solver->m_equilibrium)
			{
				printf("Solver failed!\n");
				continue;
			}
			//save
			memcpy(state + 6 * i, tensegrity[ic].m_solver->m_dx.data(), 6 * sizeof(double));
		}
		//path
		char path[200];
		printf("k: %02d\n", k);
		sprintf(path, "data/load_3_Ht_3_%02d.txt", k);
		//save
		FILE* file = fopen(path, "w");
		for(unsigned i = 0; i < nr; i++)
		{
			for(unsigned j = 0; j <= nt; j++)
			{
				//data
				const double t = 2 * M_PI * j / nt;
				const double r = tensegrity[0].m_Rr * i / nr;
				//write
				fprintf(file, "%+.6e %+.6e ", r, t);
				for(unsigned k = 0; k < 6; k++) fprintf(file, "%+.6e ", state[6 * (nt * i + j % nt) + k]);
				fprintf(file, "\n");
			}
			fprintf(file, "\n");
		}
		fclose(file);
	}
	//clock
	const time_point<high_resolution_clock> t2 = high_resolution_clock::now();
	printf("time: %.2lf s\n", double(duration_cast<milliseconds>(t2 - t1).count()) / 1e3);
	//delete
	delete[] state;
}

void test(void)
{
	const double r = 0.5;
	const uint32_t nc = 6;
	math::vector s(std::min(6U, nc + 1));
	math::matrix A(6, nc + 1), R(6, 6), Q(nc + 1, nc + 1);
	for(uint32_t i = 0; i < nc; i++)
	{
		A[0 + 6 * (i + 1)] = 0;
		A[1 + 6 * (i + 1)] = 0;
		A[5 + 6 * (i + 1)] = 0;
		A[2 + 6 * (i + 1)] = -1;
		A[3 + 6 * (i + 1)] = -r * sin(2 * M_PI * i / nc);
		A[4 + 6 * (i + 1)] = +r * cos(2 * M_PI * i / nc);
	}
	A[0 + 6 * 0] = 0;
	A[1 + 6 * 0] = 0;
	A[2 + 6 * 0] = 1;
	A[3 + 6 * 0] = 0;
	A[4 + 6 * 0] = 0;
	A[5 + 6 * 0] = 0;
	A.print("A");
	A.svd(R, Q, s);
	R.print("R");
	Q.transpose().print("Q");
	s.print("s");
}


int main(int argc, char** argv)
{
	//test
	test();
	//return
	return EXIT_SUCCESS;
}