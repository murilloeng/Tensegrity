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
static void fun_stiffness(double* K, const double* d)
{
	//data
	math::matrix Km(K, 6, 6);
	const math::vec3 x(d + 0), t(d + 3);
	const math::quat q(test_tensegrity.m_solver->m_state_old + 3);
	//state
	math::vec3(test_tensegrity.m_solver->m_state_new + 0) = x;
	math::quat(test_tensegrity.m_solver->m_state_new + 3) = q * t.quaternion();
	//force
	test_tensegrity.stiffness(Km);
}
static void fun_internal_force(double* f, const double* d)
{
	//data
	math::vector fm(f, 6);
	const math::vec3 x(d + 0), t(d + 3);
	const math::quat q(test_tensegrity.m_solver->m_state_old + 3);
	//state
	math::vec3(test_tensegrity.m_solver->m_state_new + 0) = x;
	math::quat(test_tensegrity.m_solver->m_state_new + 3) = q * t.quaternion();
	//force
	test_tensegrity.internal_force(fm);
}
static void fun_internal_energy(double* U, const double* d)
{
	//data
	const math::vec3 x(d + 0), t(d + 3);
	const math::quat q(test_tensegrity.m_solver->m_state_old + 3);
	//state
	math::vec3(test_tensegrity.m_solver->m_state_new + 0) = x;
	math::quat(test_tensegrity.m_solver->m_state_new + 3) = q * t.quaternion();
	//force
	U[0] = test_tensegrity.internal_energy();
}
static void fun_test_force(void)
{
	//data
	const unsigned nt = 10000;
	math::vector d(6), fa(6), fn(6), fr(6);
	//test
	math::quat q;
	srand(time(nullptr));
	setup(test_tensegrity);
	for(unsigned i = 0; i < nt; i++)
	{
		//setup
		d.randu();
		q.randu();
		const math::vec3 t(d.data() + 3);
		math::vector(q.data(), 4) = q.unit();
		math::quat(test_tensegrity.m_solver->m_state_old + 3) = q;
		//functions
		fun_internal_force(fa.data(), d.data());
		math::ndiff(fun_internal_energy, fn.data(), d.data(), 1, 6, 1e-8);
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
static void fun_test_stiffness(void)
{
	//data
	math::vector d(6);
	const unsigned nt = 10000;
	math::matrix Ka(6, 6), Kn(6, 6), Kr(6, 6);
	//test
	math::quat q;
	srand(time(nullptr));
	setup(test_tensegrity);
	for(unsigned i = 0; i < nt; i++)
	{
		//setup
		d.randu();
		q.randu();
		math::vector(q.data(), 4) = q.unit();
		math::quat(test_tensegrity.m_solver->m_state_old + 3) = q;
		//functions
		fun_stiffness(Ka.data(), d.data());
		math::ndiff(fun_internal_force, Kn.data(), d.data(), 6, 6, 1e-8);
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
static void load_test(void)
{
	//data
	Tensegrity tensegrity;
	const double m = 1.00e+01;
	const double g = 9.81e+00;
	const double dc = 1.50e-03;
	const double Ec = 2.00e+11;
	const double Hc = 1.40e-01;
	const double Ht = 3.20e-01;
	const double Rr = 1.40e-01;
	const double dr = 1.40e-02;
	math::vector fi(6), fe(6), r(6);
	const double Ac = M_PI * dc * dc / 4;
	//state
	const double u1 = +1.700e-04;
	const double u3 = -4.502e-05;
	const double t2 = +7.710e-04;
	//setup
	tensegrity.m_dc = dc;
	tensegrity.m_Ec = Ec;
	tensegrity.m_Hc = Hc;
	tensegrity.m_Ht = Ht;
	tensegrity.m_Rr = Rr;
	tensegrity.m_pk.push_back({0, 0, -m * g});
	tensegrity.m_ak.push_back({dr, 0, tensegrity.m_Ht});
	//state
	tensegrity.m_solver->m_state_new[0] = u1;
	tensegrity.m_solver->m_state_new[2] = u3;
	tensegrity.m_solver->m_state_new[3] = cos(t2 / 2);
	tensegrity.m_solver->m_state_new[5] = sin(t2 / 2);
	//forces
	tensegrity.external_force(fe);
	tensegrity.internal_force(fi);
	//print
	fi.transpose().print("fi");
	fe.transpose().print("fe");
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
	tensegrity.m_pk.push_back({0, 0, -m * g});
	tensegrity.m_ak.push_back({0, 0, tensegrity.m_Ht});
	double* state = (double*) alloca(7 * nr * nt * sizeof(double));
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
			printf("i: %02d j: %02d r: %+.2e t: %+.2e\n", i, j, r, t * 180 / M_PI);
			memcpy(state + 7 * (nt * i + j), tensegrity.m_solver->m_state_new, 7 * sizeof(double));
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
static void load_horizontal(void)
{
	//data
	Tensegrity tensegrity;
	const double m = 1.00e+01;
	const double g = 9.81e+00;
	using namespace std::chrono;
	const time_point<high_resolution_clock> t1 = high_resolution_clock::now();
	//setup
	setup(tensegrity);
	tensegrity.m_s0 = 1.00e+00;
	tensegrity.m_solver->m_log = true;
	tensegrity.m_solver->m_dl = 1.00e-02;
	tensegrity.m_solver->m_step_max = 100;
	tensegrity.m_solver->m_state_old[0] = 2.00e-02;
	//loads
	tensegrity.m_ak.push_back({0, 0, 0});
	tensegrity.m_pk.push_back({m * g, 0, 0});
	//solve
	tensegrity.m_solver->solve();
	//clock
	const time_point<high_resolution_clock> t2 = high_resolution_clock::now();
	printf("time: %.2lf s\n", double(duration_cast<milliseconds>(t2 - t1).count()) / 1e3);
}
static void load_moment(void)
{
	//data
	Tensegrity tensegrity;
	const double m = 1.00e+01;
	const double g = 0.91e+00;
	const double l = 1.00e+00;
	using namespace std::chrono;
	const time_point<high_resolution_clock> t1 = high_resolution_clock::now();
	//setup
	setup(tensegrity);
	tensegrity.m_s0 = 1.00e+00;
	tensegrity.m_solver->m_log = true;
	tensegrity.m_solver->m_dl = 1.00e-02;
	tensegrity.m_solver->m_step_max = 100;
	//loads
	tensegrity.m_mk.push_back({m * g * l, 0, 0});
	//solve
	tensegrity.m_solver->solve();
	//clock
	const time_point<high_resolution_clock> t2 = high_resolution_clock::now();
	printf("time: %.2lf s\n", double(duration_cast<milliseconds>(t2 - t1).count()) / 1e3);
}
int main(int argc, char** argv)
{
	//test
	load_test();
	//return
	return EXIT_SUCCESS;
}