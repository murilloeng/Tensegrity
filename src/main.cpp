//std
#include <cstdlib>

//Tensegrity
#include "inc/Solver.hpp"
#include "inc/Tensegrity.hpp"

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

static void load_vertical(void)
{
	//data
	Tensegrity tensegrity;
	const double m = 5.00e+01;
	const double g = 9.81e+00;
	//setup
	tensegrity.m_solver->m_dl = 1.00e-02;
	tensegrity.m_solver->m_step_max = 100;
	tensegrity.m_ak.push_back(math::vec3(0, 0, 0.5));
	tensegrity.m_pk.push_back([m, g] (double) { return math::vec3(0, 0, -m * g); });
	//solve
	tensegrity.m_solver->solve();
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
	//test
	force_translation(tensegrity, {1.00e-01, 0, 0}, 1000);
	//return
	return EXIT_SUCCESS;
}