//std
#include <cmath>
#include <ctime>
#include <chrono>
#include <cstdlib>

//math
#include "Math/inc/misc/misc.hpp"

//qt
#include <QtWidgets/QApplication>

//canvas
#include "Canvas/inc/Windows/Glut.hpp"

//Tensegrity
#include "Tensegrity/inc/Solver.hpp"
#include "Tensegrity/inc/Window.hpp"
#include "Tensegrity/inc/Tensegrity.hpp"

static void setup(Tensegrity& tensegrity)
{
	tensegrity.m_nc = 3;		//number of cables
	tensegrity.m_Ht = 3.20e-01; //module height
	tensegrity.m_Hc = 1.40e-01;	//central cable height
	tensegrity.m_Rr = 1.40e-01;	//plate radius
	tensegrity.m_Ec = 2.00e+11;	//cables elastic modulus
	tensegrity.m_dc = 1.50e-03;	//cables diameter
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
static void load_vertical(void)
{
	//data
	Tensegrity tensegrity;
	const unsigned nr = 100;
	const unsigned nt = 300;
	const double m = 3.00e+01;
	const double g = 9.81e+00;
	using namespace std::chrono;
	const time_point<high_resolution_clock> t1 = high_resolution_clock::now();
	//setup
	setup(tensegrity);
	double* state = new double[6 * nr * nt];
	tensegrity.m_pk.push_back({0, 0, -m * g});
	tensegrity.m_ak.push_back({0, 0, tensegrity.m_Ht});
	//loop
	for(unsigned i = 0; i < nr; i++)
	{
		for(unsigned j = 0; j < nt; j++)
		{
			//setup
			tensegrity.m_solver->clear_state();
			const double t = 2 * M_PI * j / nt;
			const double r = tensegrity.m_Rr * (i + 1) / nr;
			tensegrity.m_ak[0][0] = r * cos(t);
			tensegrity.m_ak[0][1] = r * sin(t);
			//solve
			tensegrity.m_solver->solve();
			if(!tensegrity.m_solver->m_equilibrium) return;
			//save
			printf("i: %02d j: %03d r: %+.2e t: %+.2e\n", i, j, r, t * 180 / M_PI);
			memcpy(state + 6 * (nt * i + j), tensegrity.m_solver->m_dx.data(), 6 * sizeof(double));
		}
	}
	//file
	FILE* file = fopen("data/load_vertical.txt", "w");
	for(unsigned i = 0; i < nr; i++)
	{
		for(unsigned j = 0; j <= nt; j++)
		{
			//data
			const double t = 2 * M_PI * j / nt;
			const double r = tensegrity.m_Rr * i / nr;
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
static void load_increment(void)
{
	//data
	Tensegrity tensegrity;
	const unsigned n = 100;
	const double m = 8.00e+01;
	const double g = 9.81e+00;
	double* data = new double[7 * n];
	//setup
	setup(tensegrity);
	tensegrity.m_pk.push_back({0, 0, 0});
	tensegrity.m_ak.push_back({0, 0, tensegrity.m_Ht});
	//loop
	for(unsigned i = 0; i < n; i++)
	{
		//setup
		tensegrity.m_solver->clear_state();
		tensegrity.m_pk[0][2] = -m * g * i / (n - 1);
		//solve
		tensegrity.m_solver->solve();
		if(!tensegrity.m_solver->m_equilibrium) return;
		//save
		printf("i: %02d\n", i);
		for(unsigned j = 0; j < 6; j++)
		{
			data[7 * i + j + 1] = tensegrity.m_solver->m_dx[j];
		}
		data[7 * i] = tensegrity.m_pk[0][2];
	}
	//save
	FILE* file = fopen("data/load_increment.txt", "w");
	for(unsigned i = 0; i < n; i++)
	{
		for(unsigned j = 0; j < 7; j++)
		{
			fprintf(file, "%+.2e ", data[7 * i + j]);
		}
		fprintf(file, "\n");
	}
	fclose(file);
	//delete
	delete[] data;
}
int main(int argc, char** argv)
{
	//test
	load_increment();
	// window(argc, argv);
	//return
	return EXIT_SUCCESS;
}