//std
#include <cmath>
#include <cstdint>
#include <filesystem>

//math
#include "Math/inc/misc/misc.hpp"

//tensegrity
#include "Tensegrity/inc/Map.hpp"
#include "Tensegrity/inc/tests.hpp"
#include "Tensegrity/inc/Solver.hpp"
#include "Tensegrity/inc/Strategy.hpp"
#include "Tensegrity/inc/Tensegrity.hpp"

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

void map(uint32_t mode, uint32_t nc, double fr)
{
	//data
	Map map;
	const uint32_t na = 100;
	const uint32_t nr = 100;
	const double Pr = 1.00e+03;
	//setup
	map.force(Pr);
	map.mode(mode);
	map.cables(nc);
	map.tension(fr);
	map.mesh_angle(na);
	map.mesh_radius(nr);
	//solve
	map.solve();
}

void fun_energy(double* U, const double* d, void** args)
{
	//data
	double sd[7];
	math::vec3(sd + 0) = math::vec3(d + 0);
	math::quat(sd + 3) = math::vec3(d + 3).quaternion();
	((Tensegrity*) args[0])->solver()->state(true, sd);
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
	((Tensegrity*) args[0])->solver()->state(true, sd);
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
	((Tensegrity*) args[0])->solver()->state(true, sd);
	//stiffness
	((Tensegrity*) args[0])->stiffness(Km);
}

void load_nonlinear_vertical_radial(uint32_t nc)
{
	//data
	char string[200];
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
	tensegrity.solver()->stop_condition(uint32_t(stop_condition::load_negative));
	//solve
	for(uint32_t i = 0; i < nr; i++)
	{
		//print
		printf("position: %d\n", i);
		sprintf(string, "data/vertical/radial/%d", nc);
		std::filesystem::create_directories(string);
		sprintf(string, "vertical/radial/%d/model-%d", nc, i);
		//setup
		tensegrity.label(string);
		tensegrity.load_position(0, {dp / 2 * (i + 1) / nr, 0, Ht});
		//solve
		tensegrity.solver()->solve();
	}
}
void load_nonlinear_vertical_angular(uint32_t nc)
{
	//data
	char string[200];
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
	tensegrity.solver()->stop_condition(uint32_t(stop_condition::load_negative));
	//solve
	for(uint32_t i = 0; i <= na; i++)
	{
		//print
		printf("angle: %d\n", i);
		sprintf(string, "data/vertical/angular/%d", nc);
		std::filesystem::create_directories(string);
		sprintf(string, "vertical/angular/%d/model-%d", nc, i);
		//setup
		tensegrity.label(string);
		const double t = M_PI / 3 * i / na;
		tensegrity.load_position(0, {dp / 2 * cos(t), dp / 2 * sin(t), Ht});
		//solve
		tensegrity.solver()->solve();
	}
}
void load_nonlinear_vertical_radial_peak(uint32_t nc)
{
	//data
	char string[200];
	Tensegrity tensegrity;
	const uint32_t nr = 100;
	const double fr = 1.00e+02;
	const double Pr = 1.00e+03;
	const double dl = 4.00e-02;
	const double Ht = tensegrity.height_center();
	const double dp = tensegrity.plate_diameter();
	//path
	sprintf(string, "data/vertical/radial/%d", nc);
	std::filesystem::create_directories(string);
	sprintf(string, "data/vertical/radial/%d/peak.txt", nc);
	//setup
	tensegrity.cables(nc);
	tensegrity.tension(fr);
	tensegrity.solver()->log(false);
	tensegrity.solver()->save(false);
	tensegrity.solver()->watch_dof(2);
	tensegrity.solver()->step_max(200000);
	tensegrity.solver()->load_increment(dl);
	tensegrity.add_load({0, 0, -Pr}, {dp / 2, 0, Ht});
	tensegrity.solver()->strategy(strategy_type::arc_length);
	tensegrity.solver()->stop_condition(uint32_t(stop_condition::load_decrease));
	//solve
	FILE* file = fopen(string, "w");
	for(uint32_t i = 0; i < nr; i++)
	{
		//setup
		const double r = dp / 2 * (i + 1) / nr;
		tensegrity.load_position(0, {r, 0, Ht});
		//solve
		tensegrity.solver()->solve();
		printf("step: %d radius: %+.2e\n", i, r);
		fprintf(file, "%.6e %.6e\n", r, tensegrity.solver()->load(false));
	}
	fclose(file);
}
void load_nonlinear_vertical_angular_peak(uint32_t nc)
{
	//data
	char string[200];
	Tensegrity tensegrity;
	const uint32_t na = 100;
	const double fr = 1.00e+02;
	const double Pr = 1.00e+03;
	const double dl = 4.00e-02;
	const double Ht = tensegrity.height_center();
	const double dp = tensegrity.plate_diameter();
	//path
	sprintf(string, "data/vertical/angular/%d", nc);
	std::filesystem::create_directories(string);
	sprintf(string, "data/vertical/angular/%d/peak.txt", nc);
	//setup
	tensegrity.cables(nc);
	tensegrity.tension(fr);
	tensegrity.solver()->log(false);
	tensegrity.solver()->save(false);
	tensegrity.solver()->watch_dof(2);
	tensegrity.solver()->step_max(200000);
	tensegrity.solver()->load_increment(dl);
	tensegrity.add_load({0, 0, -Pr}, {dp / 2, 0, Ht});
	tensegrity.solver()->strategy(strategy_type::arc_length);
	tensegrity.solver()->stop_condition(uint32_t(stop_condition::load_decrease));
	//solve
	FILE* file = fopen(string, "w");
	for(uint32_t i = 0; i < na; i++)
	{
		//setup
		const double a = 2 * M_PI / nc * i / na;
		tensegrity.load_position(0, {dp / 2 * cos(a), dp / 2 * sin(a), Ht});
		//solve
		tensegrity.solver()->solve();
		printf("step: %d angle: %+.2e\n", i, a);
		fprintf(file, "%.6e %.6e\n", a * nc, tensegrity.solver()->load(false));
	}
	fclose(file);
}

void load_nonlinear_horizontal_radial(uint32_t nc, bool mode)
{
	//data
	char string[200];
	Tensegrity tensegrity;
	const uint32_t nr = 10;
	const double fr = 1.00e+02;
	const double Pr = 1.00e+03;
	const double dl = 1.00e-03;
	const double Ht = tensegrity.height_total();
	const double dp = tensegrity.plate_diameter();
	//setup
	tensegrity.cables(nc);
	tensegrity.tension(fr);
	tensegrity.solver()->log(false);
	tensegrity.solver()->watch_dof(mode);
	tensegrity.solver()->step_max(200000);
	tensegrity.solver()->load_increment(dl);
	tensegrity.solver()->strategy(strategy_type::arc_length);
	tensegrity.add_load({!mode * Pr, mode * Pr, 0}, {dp / 2, 0, Ht});
	tensegrity.solver()->stop_condition(uint32_t(stop_condition::load_negative));
	//solve
	for(uint32_t i = 0; i < nr; i++)
	{
		//files
		sprintf(string, "data/horizontal-%d/radial/%d", mode, nc);
		std::filesystem::create_directories(string);
		sprintf(string, "horizontal-%d/radial/%d/model-%d", mode, nc, i);
		//setup
		tensegrity.label(string);
		tensegrity.load_position(0, {dp / 2 * (i + 1) / nr, 0, Ht});
		//solve
		tensegrity.solver()->solve();
		//print
		printf("cables: %d position: %d check: %d\n", nc, i, tensegrity.solver()->load(true) < 0);
	}
}