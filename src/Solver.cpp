//std
#include <cmath>
#include <functional>

//math
#include "Math/inc/solvers/bisection.hpp"

//tensegrity
#include "Tensegrity/inc/Solver.hpp"
#include "Tensegrity/inc/Tensegrity.hpp"

double bisection(double s, void** args)
{
	//data
	Solver* solver = (Solver*) args[0];
	const double dxr = *(double*) args[1];
	const unsigned index = *(unsigned*) args[2];
	//state
	solver->m_dx[index] = dxr + s;
	//residue
	solver->update_state();
	solver->m_tensegrity->internal_force(solver->m_fi);
	solver->m_tensegrity->external_force(solver->m_fe);
	solver->m_r = solver->m_fe - solver->m_fi;
	//return
	return solver->m_r[index];
}

//constructors
Solver::Solver(Tensegrity* tensegrity) : m_log(true), 
	m_T(1.00e+00), m_dl(1.00e-03), m_type(0), m_step_max(1000), m_iteration_max(10), 
	m_r(6), m_fn(6), m_fi(6), m_fe(6), m_Kt(6, 6), m_Ct(6, 6), m_Mt(6, 6), m_dx(6), m_dxt(6), m_ddxt(6), m_ddxr(6), m_tensegrity(tensegrity), 
	m_state_data(nullptr), m_cables_data(nullptr), m_solver_data(nullptr), m_energy_data(nullptr), m_velocity_data(nullptr), m_acceleration_data(nullptr)
{
	memset(m_state_old, 0, 7 * sizeof(double));
	memset(m_state_new, 0, 7 * sizeof(double));
	m_state_old[3] = m_state_new[3] = 1.00e+00;
	memset(m_velocity_old, 0, 6 * sizeof(double));
	memset(m_velocity_new, 0, 6 * sizeof(double));
	memset(m_acceleration_old, 0, 6 * sizeof(double));
	memset(m_acceleration_new, 0, 6 * sizeof(double));
}

//destructor
Solver::~Solver(void)
{
	delete[] m_state_data;
	delete[] m_cables_data;
	delete[] m_solver_data;
	delete[] m_energy_data;
	delete[] m_velocity_data;
	delete[] m_acceleration_data;
}

//solve
void Solver::solve(void)
{
	setup();
	!m_type ? solve_static() : solve_dynamic();
}

//solver
void Solver::setup(void)
{
	//setup
	m_step = 0;
	m_l_new = m_l_old = 0;
	m_equilibrium = false;
	if(m_type) m_tensegrity->compute_inertia();
	//memory
	delete[] m_state_data;
	delete[] m_cables_data;
	delete[] m_solver_data;
	delete[] m_energy_data;
	delete[] m_velocity_data;
	delete[] m_acceleration_data;
	m_state_data = new double[7 * (m_step_max + 1)];
	m_solver_data = new double[1 * (m_step_max + 1)];
	m_energy_data = new double[3 * (m_step_max + 1)];
	m_velocity_data = new double[6 * (m_step_max + 1)];
	m_acceleration_data = new double[6 * (m_step_max + 1)];
	m_cables_data = new double[(m_tensegrity->m_nc + 1) * (m_step_max + 1)];
	//initial
	memcpy(m_state_new, m_state_old, 7 * sizeof(double));
	memcpy(m_state_data, m_state_old, 7 * sizeof(double));
	memcpy(m_velocity_new, m_velocity_old, 6 * sizeof(double));
	memcpy(m_velocity_data, m_velocity_old, 6 * sizeof(double));
	memcpy(m_acceleration_new, m_acceleration_old, 6 * sizeof(double));
	memcpy(m_acceleration_data, m_acceleration_old, 6 * sizeof(double));
}
void Solver::record(void)
{
	m_tensegrity->compute_energy();
	memcpy(m_state_data + 7 * m_step, m_state_new, 7 * sizeof(double));
	memcpy(m_velocity_data + 6 * m_step, m_velocity_new, 6 * sizeof(double));
	memcpy(m_acceleration_data + 6 * m_step, m_acceleration_new, 6 * sizeof(double));
	//solver
	m_solver_data[m_step] = !m_type ? m_l_new : m_step * m_T / m_step_max;
	//cables
	for(unsigned i = 0; i <= m_tensegrity->m_nc; i++)
	{
		m_cables_data[(m_tensegrity->m_nc + 1) * m_step + i] = 0;
	}
}
void Solver::finish(void)
{
	//data
	const unsigned sizes[] = {7, m_tensegrity->m_nc + 1, 1, 3, 6, 6};
	const char* labels[] = {"state", "cables", "solver", "energy", "velocity", "acceleration"};
	const double* data[] = {m_state_data, m_cables_data, m_solver_data, m_energy_data, m_velocity_data, m_acceleration_data};
	//write
	for(unsigned i = 0; i < 6; i++)
	{
		//path
		char path[200];
		sprintf(path, "data/%s_%s.txt", m_tensegrity->m_label, labels[i]);
		//file
		FILE* file = fopen(path, "w");
		//write
		for(unsigned j = 0; j <= m_step_max; j++)
		{
			for(unsigned k = 0; k < sizes[i]; k++)
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
	memcpy(m_velocity_old, m_velocity_new, 6 * sizeof(double));
	memcpy(m_acceleration_old, m_acceleration_new, 6 * sizeof(double));
}
void Solver::restore(void)
{
	m_l_new = m_l_old;
	memcpy(m_state_new, m_state_old, 7 * sizeof(double));
	memcpy(m_velocity_new, m_velocity_old, 6 * sizeof(double));
	memcpy(m_acceleration_new, m_acceleration_old, 6 * sizeof(double));
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
void Solver::solve_static(void)
{
	//setup
	m_dx.zeros();
	m_tensegrity->internal_force(m_fi);
	m_tensegrity->external_force(m_fe);
	//data
	double dxr;
	unsigned index = 0;
	math::bisection solver;
	const double fr = m_fe.norm();
	void* args[] = {this, &dxr, &index};
	//solver
	m_r = m_fe - m_fi;
	solver.m_system = bisection;
	solver.m_tolerance = 1e-5 * fr;
	//loop
	for(m_iteration = 0; m_iteration < m_iteration_max; m_iteration++)
	{
		if(fabs(m_r[index]) > solver.m_tolerance)
		{
			dxr = m_dx[index];
			solver.m_x1 = solver.m_x2 = 0;
			m_tensegrity->stiffness(m_Kt);
			double f1 = bisection(solver.m_x1, args);
			double f2 = bisection(solver.m_x2, args);
			double ds = m_r[index] / m_Kt[7 * index];
			while(f1 * f2 > 0)
			{
				solver.m_x2 += ds;
				f2 = bisection(solver.m_x2, args);
			}
			if(!solver.solve(args))
			{
				return;
			}
			m_dx[index] = dxr + solver.m_xs;
			if(m_r.max(true) < solver.m_tolerance) break;
		}
		index = (index + 1) % 6;
	}
	m_equilibrium = true;
}
void Solver::solve_dynamic(void)
{
	return;
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