//std
#include <cmath>
#include <cstdio>
#include <cstring>
#include <functional>

//tensegrity
#include "Tensegrity/inc/Solver.hpp"
#include "Tensegrity/inc/Strategy.hpp"
#include "Tensegrity/inc/Tensegrity.hpp"

//constructors
Solver::Solver(Tensegrity* tensegrity) :
	m_r(6), m_fn(6), m_fi(6), m_fe(6), m_Kt(6, 6), m_Ct(6, 6), m_Mt(6, 6), m_dx(6), m_dxt(6), m_ddxt(6), m_ddxr(6), m_tensegrity(tensegrity)
{
	m_log = true;
	m_save = true;
	m_dl0 = 1.00e-03;
	m_step_max = 1000;
	m_iteration_max = 10;
	m_stop_condition = 0;
	m_strategy = nullptr;
	m_equilibrium = false;
	m_state_data = nullptr;
	m_energy_data = nullptr;
	m_cables_data = nullptr;
	m_solver_data = nullptr;
	m_state_old = new double[7];
	m_state_new = new double[7];
	memset(m_state_old, 0, 7 * sizeof(double));
	memset(m_state_new, 0, 7 * sizeof(double));
	m_state_old[3] = m_state_new[3] = 1.00e+00;
	Strategy::create(m_strategy, strategy_type::control_load);
}

//destructor
Solver::~Solver(void)
{
	delete m_strategy;
	delete[] m_state_old;
	delete[] m_state_new;
	delete[] m_state_data;
	delete[] m_cables_data;
	delete[] m_solver_data;
	delete[] m_energy_data;
}

//data
bool Solver::log(bool log)
{
	return m_log = log;
}
bool Solver::log(void) const
{
	return m_log;
}

bool Solver::save(bool save)
{
	return m_save = save;
}
bool Solver::save(void) const
{
	return m_save;
}

bool Solver::equilibrium(void) const
{
	return m_equilibrium;
}

Strategy* Solver::strategy(void) const
{
	return m_strategy;
}
Strategy* Solver::strategy(strategy_type type)
{
	return Strategy::create(m_strategy, type);
}

uint32_t Solver::step_max(void) const
{
	return m_step_max;
}
uint32_t Solver::step_max(uint32_t step_max)
{
	return m_step_max = step_max;
}

uint32_t Solver::watch_dof(void) const
{
	return m_watch_dof;
}
uint32_t Solver::watch_dof(uint32_t watch_dof)
{
	return m_watch_dof = watch_dof;
}

uint32_t Solver::iteration_max(void) const
{
	return m_iteration_max;
}
uint32_t Solver::iteration_max(uint32_t iteration_max)
{
	return m_iteration_max = iteration_max;
}

uint32_t Solver::stop_condition(void) const
{
	return m_stop_condition;
}
uint32_t Solver::stop_condition(uint32_t stop_condition)
{
	return m_stop_condition = stop_condition;
}

double Solver::load_increment(double dl0)
{
	return m_dl0 = dl0;
}
double Solver::load_increment(void) const
{
	return m_dl0;
}

double Solver::load(bool step) const
{
	return step ? m_l_new : m_l_old;
}
double Solver::load(bool step, double load)
{
	return (step ? m_l_new : m_l_old) = load;
}

const double* Solver::state(bool step) const
{
	return step ? m_state_new : m_state_old;
}
const double* Solver::state(bool step, const double* state)
{
	memcpy(step ? m_state_new : m_state_old, state, 7 * sizeof(double));
	return step ? m_state_new : m_state_old;
}

//increments
double Solver::load_predictor(void) const
{
	return m_dl;
}
double Solver::load_corrector(void) const
{
	return m_ddl;
}

const math::vector& Solver::dof_predictor(void) const
{
	return m_dxt;
}
const math::vector& Solver::dof_increment(void) const
{
	return m_dx;
}
const math::vector& Solver::dof_corrector(bool type) const
{
	return type ? m_ddxt : m_ddxr;
}

//operators
Solver& Solver::operator=(const Solver& solver)
{
	m_dl = solver.m_dl;
	m_log = solver.m_log;
	m_step_max = solver.m_step_max;
	m_iteration_max = solver.m_iteration_max;
	Strategy::create(m_strategy, solver.strategy()->type());
	return *this;
}

//solve
void Solver::solve(void)
{
	setup();
	record();
	for(m_step = 1; m_step <= m_step_max; m_step++)
	{
		predictor();
		for(m_iteration = 0; m_iteration < m_iteration_max; m_iteration++)
		{
			//state
			m_tensegrity->internal_force(m_fi);
			m_tensegrity->external_force(m_fe);
			//residue
			m_r = m_l_new * m_fe - m_fi;
			if(m_r.norm() < 1e-5 * m_fe.norm())
			{
				stop();
				update();
				record();
				log_step();
				break;
			}
			//update
			corrector();
		}
		if(m_stop) break;
	}
	finish();
}

//solver
void Solver::stop(void)
{
	//data
	const bool c1 = m_l_new < 0;
	const bool c2 = m_l_new < m_l_old;
	const bool c3 = m_dx[m_watch_dof] < 0;
	//check
	m_stop = false;
	m_equilibrium = true;
	m_stop = m_stop || (m_stop_condition & uint32_t(::stop_condition::dof_decrease) && c3);
	m_stop = m_stop || (m_stop_condition & uint32_t(::stop_condition::load_decrease) && c2);
	m_stop = m_stop || (m_stop_condition & uint32_t(::stop_condition::load_negative) && c1);
}
void Solver::setup(void)
{
	//setup
	m_step = 0;
	m_equilibrium = false;
	m_l_new = m_l_old = 0;
	//memory
	clear_state();
	delete[] m_state_data;
	delete[] m_cables_data;
	delete[] m_solver_data;
	delete[] m_energy_data;
	m_state_data = new double[7 * (m_step_max + 1)];
	m_solver_data = new double[1 * (m_step_max + 1)];
	m_energy_data = new double[3 * (m_step_max + 1)];
	m_cables_data = new double[(m_tensegrity->m_nc + 1) * (m_step_max + 1)];
}
void Solver::record(void)
{
	if(!m_save) return;
	m_solver_data[m_step] = m_l_new;
	m_energy_data[3 * m_step + 0] = 0;
	m_energy_data[3 * m_step + 1] = 0;
	m_energy_data[3 * m_step + 2] = 0;
	for(uint32_t k = 0; k <= m_tensegrity->m_nc; k++)
	{
		m_cables_data[(m_tensegrity->m_nc + 1) * m_step + k] = m_tensegrity->cable_force(k);
	}
	memcpy(m_state_data + 7 * m_step, m_state_new, 7 * sizeof(double));
}
void Solver::finish(void)
{
	//data
	if(!m_save) return;
	const uint32_t sizes[] = {7, m_tensegrity->m_nc + 1, 1, 3};
	const char* labels[] = {"state", "cables", "solver", "energy"};
	const double* data[] = {m_state_data, m_cables_data, m_solver_data, m_energy_data};
	//write
	for(uint32_t i = 0; i < sizeof(sizes) / sizeof(uint32_t); i++)
	{
		//path
		char path[200];
		sprintf(path, "data/%s-%s.txt", m_tensegrity->m_label, labels[i]);
		//file
		FILE* file = fopen(path, "w");
		//write
		for(uint32_t j = 0; j <= std::min(m_step, m_step_max); j++)
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
void Solver::log_step(void)
{
	if(m_log)
	{
		const math::vec3 r = math::quat(m_state_new + 3).pseudo();
		const double dof = m_watch_dof < 3 ? m_state_new[m_watch_dof] : r[m_watch_dof - 3];
		printf("step: %04d iterations: %02d load: %+.2e dof: %+.2e\n", m_step, m_iteration, m_l_new, dof);
	}
}
void Solver::predictor(void)
{
	//state
	m_stop = true;
	m_equilibrium = false;
	m_tensegrity->stiffness(m_Kt);
	m_tensegrity->external_force(m_fe);
	//tangent
	m_Kt.solve(m_dxt, m_fe);
	m_dl = m_step == 1 ? m_dl0 : m_strategy->load_predictor(this);
	//update
	m_dx = m_dl * m_dxt;
	m_l_new = m_l_old + m_dl;
}
void Solver::corrector(void)
{
	//state
	m_tensegrity->stiffness(m_Kt);
	//increments
	m_Kt.solve(m_ddxr, m_r);
	m_Kt.solve(m_ddxt, m_fe);
	m_ddl = m_strategy->load_corrector(this);
	//increment
	m_dl += m_ddl;
	m_dx += m_ddxr + m_ddl * m_ddxt;
	//update
	update_state();
	m_l_new = m_l_old + m_dl;
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