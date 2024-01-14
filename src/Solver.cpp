//math
#include "../Math/inc/misc/misc.hpp"

//tensegrity
#include "inc/Solver.hpp"
#include "inc/Tensegrity.hpp"

//constructors
Solver::Solver(Tensegrity* tensegrity) : 
	m_tensegrity(tensegrity), m_step_max(1000), m_type(0), m_iteration_max(10), m_T(1.00e+00), 
	m_state_data(nullptr), m_energy_data(nullptr), m_velocity_data(nullptr), m_acceleration_data(nullptr),
	m_r(6), m_fn(6), m_fi(6), m_fe(6), m_Kt(6, 6), m_Ct(6, 6), m_Mt(6, 6), m_dx(6), m_dxt(6), m_ddxt(6), m_ddxr(6)
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
	m_l_new = m_l_old = 0;
	m_tensegrity->compute_mass();
	m_tensegrity->compute_center();
	m_tensegrity->compute_inertia();
	//memory
	delete[] m_state_data;
	delete[] m_energy_data;
	delete[] m_velocity_data;
	delete[] m_acceleration_data;
	m_state_data = new double[7 * (m_step_max + 1)];
	m_energy_data = new double[3 * (m_step_max + 1)];
	m_velocity_data = new double[6 * (m_step_max + 1)];
	m_acceleration_data = new double[6 * (m_step_max + 1)];
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
}
void Solver::finish(void)
{

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
	//loop
	m_tensegrity->stiffness(m_Kt);
	m_tensegrity->external_force(m_fe);
	for(m_step = 0; m_step < m_step_max; m_step++)
	{
		//predictor
		m_Kt.solve(m_dxt, m_fe);
		compute_load_predictor();
		m_dx = m_dl * m_dxt;
		//update
		update_state();
		m_l_new = m_l_old + m_dl;
		//corrector
		for(m_iteration = 0; m_iteration < m_iteration_max; m_iteration++)
		{
			m_tensegrity->stiffness(m_Kt);
			m_tensegrity->internal_force(m_fi);
			m_r = m_l_new * m_fe - m_fi;
			if(m_r.norm() < 1e-5 * m_fe.norm())
			{
				printf("step: %04d iterations: %04d load: %+.2e\n", m_step, m_iteration, m_l_new);
				record();
				update();
				break;
			}
			m_Kt.solve(m_ddxr, m_r);
			m_Kt.solve(m_ddxt, m_fe);
			compute_load_corrector();
			m_dl += m_ddl;
			m_dx += m_ddxr + m_ddl * m_ddxt;
			update_state();
			m_l_new = m_l_old + m_dl;
		}
	}
}
void Solver::solve_dynamic(void)
{

}

//formulation
void Solver::compute_load_predictor(void)
{
	if(m_step != 0)
	{
		m_dl = math::sign(m_dxt.inner(m_dx)) * m_dx.norm() / m_dxt.norm();
	}
}
void Solver::compute_load_corrector(void)
{
	const double s = m_ddxt.inner(m_dx);
	const double a = m_ddxt.inner(m_ddxt);
	const double b = m_ddxt.inner(m_ddxr + m_dx);
	const double c = m_ddxr.inner(m_ddxr + 2 * m_dx);
	m_ddl = -b / a + math::sign(s) * sqrt(pow(b / a, 2) - c / a);
}