//std
#include <cmath>
#include <ctime>
#include <chrono>
#include <cstdlib>

//math
#include "Math/inc/misc/misc.hpp"

//canvas
#include "Canvas/inc/Windows/Glut.hpp"

//Tensegrity
#include "Tensegrity/inc/Solver.hpp"
#include "Tensegrity/inc/Tensegrity.hpp"

static Tensegrity test_tensegrity;

static void setup(Tensegrity& tensegrity)
{
	tensegrity.m_nc = 3;
	tensegrity.m_Ht = 3.20e-01;
	tensegrity.m_Hc = 1.40e-01;
	tensegrity.m_Rr = 1.40e-01;
	tensegrity.m_Ec = 2.00e+11;
	tensegrity.m_dc = 1.50e-03;
	tensegrity.m_s0 = 0.00e+00;
}
static void draw(const Tensegrity& tensegrity, int argc, char** argv)
{
	//data
	canvas::windows::Glut window(argc, argv, "../Canvas/shd/");
	//draw
	tensegrity.draw_model(window.scene());
	//update
	window.scene()->update(true);
	//main loop
	window.start();
}
static void energy(void)
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
			fprintf(file, "%+.6e %+.6e ", u, tensegrity.internal_energy());
		}
		fprintf(file, "\n");
	}
	//close
	fclose(file);
}
static void internal_force(void)
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
			fprintf(file, "%+.6e ", u);
			for(unsigned k = 0; k < 6; k++) fprintf(file, "%+.6e ", fi[k]);
		}
		fprintf(file, "\n");
	}
	//close
	fclose(file);
}
void fun_stiffness(double* U, const double* x)
{
	
}
void fun_internal_force(double* U, const double* x)
{
	
}
void fun_internal_energy(double* U, const double* x)
{

}
static void load_vertical(void)
{
	//data
	Tensegrity tensegrity;
	const unsigned nr = 100;
	const unsigned nt = 100;
	const double m = 1.00e+01;
	const double g = 9.81e+00;
	using namespace std::chrono;
	const time_point<high_resolution_clock> t1 = high_resolution_clock::now();
	//setup
	setup(tensegrity);
	tensegrity.m_s0 = 1.00e+00;
	tensegrity.m_solver->m_log = false;
	tensegrity.m_solver->m_dl = 1.00e-02;
	tensegrity.m_solver->m_step_max = 100;
	tensegrity.m_ak.push_back({0, 0, tensegrity.m_Ht});
	tensegrity.m_pk.push_back([m, g] (double) { return math::vec3(0, 0, -m * g); });
	double* state = (double*) alloca(7 * nr * nt * sizeof(double));
	//loop
	for(unsigned i = 1; i < nr; i++)
	{
		for(unsigned j = 0; j < nt; j++)
		{
			//setup
			tensegrity.m_solver->clear_state();
			const double t = 2 * M_PI * j / nt;
			const double r = tensegrity.m_Rr * i / nr;
			tensegrity.m_ak[0][0] = r * cos(t);
			tensegrity.m_ak[0][1] = r * sin(t);
			tensegrity.m_K0[5] = (r == 0) * 1.00e+03;
			//solve
			tensegrity.m_solver->solve();
			if(!tensegrity.m_solver->m_equilibrium) return;
			//save
			printf("i: %02d j: %02d r: %+.2e t: %+.2e\n", i, j, r, t * 180 / M_PI);
			memcpy(state + 7 * (nt * i + j), tensegrity.m_solver->m_state_new, 7 * sizeof(double));
		}
	}
	//file
	FILE* file = fopen("load_vertical.txt", "w");
	for(unsigned i = 0; i < nr; i++)
	{
		for(unsigned j = 0; j <= nt; j++)
		{
			//data
			const double t = 2 * M_PI * j / nt;
			const double r = tensegrity.m_Rr * i / nr;
			const math::vec3 u = math::vec3(state + 7 * (nt * i + j % nt) + 0);
			const math::vec3 v = math::quat(state + 7 * (nt * i + j % nt) + 3).pseudo();
			//write
			fprintf(file, "%+.6e %+.6e ", r, t);
			for(unsigned k = 0; k < 3; k++) fprintf(file, "%+.6e ", u[k]);
			for(unsigned k = 0; k < 3; k++) fprintf(file, "%+.6e ", v[k]);
			fprintf(file, "\n");
		}
		fprintf(file, "\n");
	}
	fclose(file);
	//clock
	const time_point<high_resolution_clock> t2 = high_resolution_clock::now();
	printf("time: %.2lf s\n", double(duration_cast<milliseconds>(t2 - t1).count()) / 1e3);
}

void test_energy(void)
{
	//data
	Tensegrity tensegrity;
	//setup
	tensegrity.m_nc = 3;
	tensegrity.m_Ht = 3.20e-01;
	tensegrity.m_Hc = 1.40e-01;
	tensegrity.m_Rr = 1.40e-01;
	tensegrity.m_Ec = 2.00e+11;
	tensegrity.m_dc = 1.50e-03;
	tensegrity.m_s0 = 0.00e+00;
	//energy
	const double t = 1e-5;
	tensegrity.m_solver->m_state_new[3] = cos(t / 2);
	tensegrity.m_solver->m_state_new[5] = sin(t / 2);
	printf("U: %+.2e\n", tensegrity.internal_energy());
}

int main(int argc, char** argv)
{
	//test
	if(argc != 1)
	{
		if(atoi(argv[1]) == 0)
		{
			energy();
		}
		else
		{
			internal_force();
		}
	}
	//return
	return EXIT_SUCCESS;
}