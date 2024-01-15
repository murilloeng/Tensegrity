//std
#include <cstdlib>

//Tensegrity
#include "inc/Solver.hpp"
#include "inc/Tensegrity.hpp"

static void rotation(math::vec3 t)
{
	//data
	math::vector fi(6);
	Tensegrity tensegrity;
	const unsigned ns = 100;
	//state
	tensegrity.m_nc = 4;
	for(unsigned i = 0; i <= ns; i++)
	{
		//state
		math::quat(tensegrity.m_solver->m_state_new + 3) = t.quaternion();
		//formulation
		printf("%03d ", i);
		tensegrity.internal_force(fi);
		for(unsigned i = 0; i < 6; i++)
		{
			printf("%+.2e ", fi[i]);
		}
		printf("\n");
	}
}
static void translation(math::vec3 u)
{
	//data
	math::vector fi(6);
	Tensegrity tensegrity;
	const unsigned ns = 100;
	//state
	tensegrity.m_nc = 4;
	for(unsigned i = 0; i <= ns; i++)
	{
		//state
		math::vec3(tensegrity.m_solver->m_state_new + 0) = i * u / ns;
		//formulation
		printf("%03d ", i);
		tensegrity.internal_force(fi);
		for(unsigned i = 0; i < 6; i++)
		{
			printf("%+.2e ", fi[i]);
		}
		printf("\n");
	}
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
	//test
	translation({1.00e-01, 0, 0});
	//return
	return EXIT_SUCCESS;
}