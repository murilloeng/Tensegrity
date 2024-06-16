//std
#include <cmath>
#include <ctime>
#include <cstdlib>

//math
#include "Math/inc/misc/misc.hpp"

//canvas
#include "Canvas/inc/Windows/Glut.hpp"

//Tensegrity
#include "Tensegrity/inc/Solver.hpp"
#include "Tensegrity/inc/Tensegrity.hpp"

static void force_rotation(Tensegrity& tensegrity, math::vec3 t, unsigned ns)
{
	//data
	math::vector fi(6);
	FILE* file = fopen("data/force_rotation.txt", "w");
	//loop
	for(unsigned i = 0; i <= ns; i++)
	{
		//state
		const math::vec3 ts = double(i) / ns * t;
		math::quat(tensegrity.m_solver->m_state_new + 3) = ts.quaternion();
		//formulation
		tensegrity.internal_force(fi);
		fprintf(file, "%+.6e %+.6e %+.6e ", ts[0], ts[1], ts[2]);
		fprintf(file, "%+.6e %+.6e %+.6e ", fi[0], fi[1], fi[2]);
		fprintf(file, "%+.6e %+.6e %+.6e ", fi[3], fi[4], fi[5]);
		fprintf(file, "\n");
	}
	//close
	fclose(file);
}
static void force_translation(Tensegrity& tensegrity, math::vec3 u, unsigned ns)
{
	//data
	math::vector fi(6);
	FILE* file = fopen("data/force_translation.txt", "w");
	//loop
	for(unsigned i = 0; i <= ns; i++)
	{
		//state
		const math::vec3 ts = double(i) / ns * u;
		math::vec3(tensegrity.m_solver->m_state_new + 0) = ts;
		//formulation
		tensegrity.internal_force(fi);
		fprintf(file, "%+.6e %+.6e %+.6e ", ts[0], ts[1], ts[2]);
		fprintf(file, "%+.6e %+.6e %+.6e ", fi[0], fi[1], fi[2]);
		fprintf(file, "%+.6e %+.6e %+.6e ", fi[3], fi[4], fi[5]);
		fprintf(file, "\n");
	}
	//close
	fclose(file);
}

static void energy_rotation(Tensegrity& tensegrity, math::vec3 t, unsigned ns)
{
	//data
	FILE* file = fopen("data/energy_rotation.txt", "w");
	//loop
	for(unsigned i = 0; i <= ns; i++)
	{
		math::vec3 ts = (2 * double(i) / ns - 1) * t;
		math::quat(tensegrity.m_solver->m_state_new + 3) = ts.quaternion();
		fprintf(file, "%+.6e %+.6e %+.6e %+.6e\n", ts[0], ts[1], ts[2], tensegrity.internal_energy());
	}
	//close
	fclose(file);
}
static void energy_translation(Tensegrity& tensegrity, math::vec3 u, unsigned ns)
{
	//data
	FILE* file = fopen("data/energy_translation.txt", "w");
	//loop
	for(unsigned i = 0; i <= ns; i++)
	{
		math::vec3 us = (2 * double(i) / ns - 1) * u;
		math::vec3(tensegrity.m_solver->m_state_new) = us;
		fprintf(file, "%+.6e %+.6e %+.6e %+.6e\n", us[0], us[1], us[2], tensegrity.internal_energy());
	}
	//close
	fclose(file);
}

static Tensegrity test_tensegrity;
static void fun(double* f, const double* d)
{
	//data
	math::vector fi(6), fe(6);
	math::vec3 x_new(test_tensegrity.m_solver->m_state_new + 0);
	math::quat q_new(test_tensegrity.m_solver->m_state_new + 3);
	const math::quat q_old(test_tensegrity.m_solver->m_state_old + 3);
	//setup
	x_new = math::vec3(d + 0);
	q_new = q_old * math::vec3(d + 3).quaternion();
	//function
	test_tensegrity.internal_force(fi);
	test_tensegrity.external_force(fe);
	math::vector(f, 6) = fi - test_tensegrity.m_solver->m_l_new * fe;
}
static void dfun(double* K, const double* d)
{
	//data
	math::matrix Km(K, 6, 6);
	math::vec3 x_new(test_tensegrity.m_solver->m_state_new + 0);
	math::quat q_new(test_tensegrity.m_solver->m_state_new + 3);
	const math::quat q_old(test_tensegrity.m_solver->m_state_old + 3);
	//setup
	x_new = math::vec3(d + 0);
	q_new = q_old * math::vec3(d + 3).quaternion();
	//function
	test_tensegrity.stiffness(Km);
}
static void test(void)
{
	//data
	math::vec3 tn;
	math::vector x(6), fi(6);
	math::matrix Ka(6, 6), Kn(6, 6), Kr(6, 6);
	//setup
	srand(time(nullptr));
	const double r = 1.00e-01;
	const double t = 2.00e-01;
	const double m = 1.00e+01;
	const double g = 9.81e+00;
	const unsigned nt = 100000;
	test_tensegrity.m_Ht = 3.20e-01;
	test_tensegrity.m_Hc = 1.40e-01;
	test_tensegrity.m_Rr = 1.40e-01;
	test_tensegrity.m_Ec = 2.00e+11;
	test_tensegrity.m_dc = 1.50e-03;
	test_tensegrity.m_s0 = 1.00e+02;
	test_tensegrity.m_ak.push_back({r * cos(t), r * sin(t), test_tensegrity.m_Ht});
	test_tensegrity.m_pk.push_back([m, g] (double) { return math::vec3(0, 0, -m * g); });
	for(unsigned i = 0; i < nt; i++)
	{
		//state
		x.randu();
		tn.randu();
		math::vector(&test_tensegrity.m_solver->m_l_new, 1).randu(0, 1);
		math::quat(test_tensegrity.m_solver->m_state_old + 3) = tn.quaternion();
		//function
		dfun(Ka.data(), x.data());
		math::ndiff(fun, Kn.data(), x.data(), 6, 6, 1e-8);
		//check
		if((Ka - Kn).norm() < 1e-5 * Ka.norm())
		{
			printf("%d ok\n", i);
		}
		else
		{
			x.print("x");
			math::vector(test_tensegrity.m_solver->m_state_old, 7).print("state old");
			math::vector(test_tensegrity.m_solver->m_state_new, 7).print("state new");
			Ka.print("Ka");
			Kn.print("Kn");
			(Ka - Kn).print("Ka - Kn", 1e-7 * Ka.norm());
			printf("%d not ok\n", i);
			break;
		}
	}
}
static void load_vertical(void)
{
	//data
	Tensegrity tensegrity;
	const unsigned nr = 100;
	const unsigned nt = 100;
	const double m = 1.00e+01;
	const double g = 9.81e+00;
	//setup
	tensegrity.m_nc = 3;
	tensegrity.m_Ht = 3.20e-01;
	tensegrity.m_Hc = 1.40e-01;
	tensegrity.m_Rr = 1.40e-01;
	tensegrity.m_Ec = 2.00e+11;
	tensegrity.m_dc = 1.50e-03;
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
}
static void load_placement(void)
{
	//data
	Tensegrity tensegrity;
	const unsigned nr = 20;
	const unsigned nt = 80;
	const double m = 5.00e+01;
	const double g = 9.81e+00;
	FILE* file = fopen("data/load_placement.txt", "w");
	//setup
	tensegrity.m_solver->m_log = false;
	tensegrity.m_ak.push_back(math::vec3(0, 0, 0.5));
	tensegrity.m_pk.push_back([m, g] (double) { return math::vec3(0, 0, -m * g); });
	//solve
	for(unsigned i = 0; i < nt; i++)
	{
		for(unsigned j = 0; j <= nr; j++)
		{
			//data
			const double t = 2 * i * M_PI / nt;
			const double r = j * tensegrity.m_Rr / nr;
			//setup
			tensegrity.m_ak[0] = math::vec3(r * cos(t), r * sin(t), 0.5);
			//solve
			tensegrity.m_solver->solve();
			//print
			printf("%03d %03d %+.2e %+.2e ", i, j, r * cos(t), r * sin(t));
			fprintf(file, "%03d %03d %+.6e %+.6e ", i, j, r * cos(t), r * sin(t));
			for(unsigned k = 0; k < 7; k++)
			{
				printf("%+.2e ", tensegrity.m_solver->m_state_old[k]);
				fprintf(file, "%+.6e ", tensegrity.m_solver->m_state_old[k]);
			}
			printf("\n");
			fprintf(file, "\n");
		}
	}
	//close
	fclose(file);
}

int main(int argc, char** argv)
{
	//data
	Tensegrity tensegrity;
	// canvas::windows::Glut window(argc, argv, "../Canvas/shd/");
	//draw
	// tensegrity.draw_model(window.scene());
	// window.scene()->update(true);
	// window.start();
	//test
	// test();
	load_vertical();
	//return
	return EXIT_SUCCESS;
}