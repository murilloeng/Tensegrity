//std
#include <cmath>
#include <functional>

//tensegrity
#include "Tensegrity/inc/Solver.hpp"
#include "Tensegrity/inc/Tensegrity.hpp"

//constructors
Solver::Solver(Tensegrity* tensegrity) : m_log(true), 
	m_T(1.00e+00), m_dl(1.00e-03), m_type(0), m_step_max(1000), m_iteration_max(10), 
	m_r(6), m_fn(6), m_fi(6), m_fe(6), m_Kt(6, 6), m_Ct(6, 6), m_Mt(6, 6), m_dx(6), m_dxt(6), m_ddxt(6), m_ddxr(6), m_tensegrity(tensegrity), m_state_data(nullptr), m_cables_data(nullptr), m_solver_data(nullptr), m_energy_data(nullptr)
{
	memset(m_state_old, 0, 7 * sizeof(double));
	memset(m_state_new, 0, 7 * sizeof(double));
	m_state_old[3] = m_state_new[3] = 1.00e+00;
}

//destructor
Solver::~Solver(void)
{
	delete[] m_state_data;
	delete[] m_cables_data;
	delete[] m_solver_data;
	delete[] m_energy_data;
}

//solve
void Solver::solve(void)
{
	return;
}

//solver
void Solver::setup(void)
{
	//setup
	m_step = 0;
	m_l_new = m_l_old = 0;
	m_equilibrium = false;
	//memory
	delete[] m_state_data;
	delete[] m_cables_data;
	delete[] m_solver_data;
	delete[] m_energy_data;
	m_state_data = new double[7 * (m_step_max + 1)];
	m_solver_data = new double[1 * (m_step_max + 1)];
	m_energy_data = new double[3 * (m_step_max + 1)];
	m_cables_data = new double[(m_tensegrity->m_nc + 1) * (m_step_max + 1)];
	//initial
	memcpy(m_state_new, m_state_old, 7 * sizeof(double));
	memcpy(m_state_data, m_state_old, 7 * sizeof(double));
}
void Solver::record(void)
{
	// m_tensegrity->compute_energy();
	memcpy(m_state_data + 7 * m_step, m_state_new, 7 * sizeof(double));
	//solver
	m_solver_data[m_step] = !m_type ? m_l_new : m_step * m_T / m_step_max;
	//cables
	for(uint32_t i = 0; i <= m_tensegrity->m_nc; i++)
	{
		m_cables_data[(m_tensegrity->m_nc + 1) * m_step + i] = 0;
	}
}
void Solver::finish(void)
{
	//data
	const uint32_t sizes[] = {7, m_tensegrity->m_nc + 1, 1, 3};
	const char* labels[] = {"state", "cables", "solver", "energy"};
	const double* data[] = {m_state_data, m_cables_data, m_solver_data, m_energy_data};
	//write
	for(uint32_t i = 0; i < 6; i++)
	{
		//path
		char path[200];
		sprintf(path, "data/%s_%s.txt", m_tensegrity->m_label, labels[i]);
		//file
		FILE* file = fopen(path, "w");
		//write
		for(uint32_t j = 0; j <= m_step_max; j++)
		{
			for(uint32_t k = 0; k < sizes[i]; k++)
			{
				fprintf(file, "%+.6e ", data[i][sizes[i] * j + k]);
			}
			fprintf(file, "\n");
		}
		//close
		fclose(file);
	}
}
void Solver::update(void)
{
	m_l_old = m_l_new;
	memcpy(m_state_old, m_state_new, 7 * sizeof(double));
}
void Solver::restore(void)
{
	m_l_new = m_l_old;
	memcpy(m_state_new, m_state_old, 7 * sizeof(double));
}
void Solver::clear_state(void)
{
	memset(m_state_old, 0, 7 * sizeof(double));
	memset(m_state_new, 0, 7 * sizeof(double));
	m_state_old[3] = m_state_new[3] = 1;
}
void Solver::update_state(void)
{
	//data
	math::vec3 u_new(m_state_new + 0);
	math::quat q_new(m_state_new + 3);
	const math::vec3 u_old(m_state_old + 0);
	const math::quat q_old(m_state_old + 3);
	const math::vec3 du_new(m_dx.data() + 0);
	const math::vec3 dt_new(m_dx.data() + 3);
	//update
	u_new = u_old + du_new;
	q_new = q_old * dt_new.quaternion();
}

//formulation
void Solver::compute_residue(void)
{
	//state
	update_state();
	m_tensegrity->stiffness(m_Kt);
	m_tensegrity->internal_force(m_fi);
	m_tensegrity->external_force(m_fe);
	//residue
	m_r = m_fe - m_fi;
}
void Solver::compute_load_predictor(void)
{
	return;
	// if(m_step != 1)
	// {
	// 	m_dl = math::sign(m_dxt.inner(m_dx)) * m_dx.norm() / m_dxt.norm();
	// }
}
void Solver::compute_load_corrector(void)
{
	m_ddl = 0;
	// const double s = m_ddxt.inner(m_dx);
	// const double a = m_ddxt.inner(m_ddxt);
	// const double b = m_ddxt.inner(m_ddxr + m_dx);
	// const double c = m_ddxr.inner(m_ddxr + 2 * m_dx);
	// m_ddl = -b / a + math::sign(s) * sqrt(pow(b / a, 2) - c / a);
}