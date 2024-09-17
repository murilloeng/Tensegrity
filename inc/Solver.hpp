#pragma once

//std
#include <cstdint>

//math
#include "Math/inc/linear/vector.hpp"

class Deformed;
class Strategy;
class Tensegrity;
enum class strategy_type : uint32_t;

class Solver
{
	//friends
	friend class Tensegrity;

public:
	//constructors
	Solver(Tensegrity*);

	//destructor
	~Solver(void);

	//data
	bool log(bool);
	bool log(void) const;

	bool save(bool);
	bool save(void) const;

	bool equilibrium(void) const;

	Strategy* strategy(void) const;
	Strategy* strategy(strategy_type);

	uint32_t step_max(uint32_t);
	uint32_t step_max(void) const;

	uint32_t watch_dof(uint32_t);
	uint32_t watch_dof(void) const;

	uint32_t iteration_max(uint32_t);
	uint32_t iteration_max(void) const;

	double load_increment(double);
	double load_increment(void) const;

	const double* state(void) const;
	const double* state(const double*);

	//increments
	double load_corrector(void) const;
	double load_predictor(void) const;

	const math::vector& dof_predictor(void) const;
	const math::vector& dof_increment(void) const;
	const math::vector& dof_corrector(bool) const;

	//operators
	Solver& operator=(const Solver&);

	//solve
	void solve(void);

private:
	//solver
	void setup(void);
	void record(void);
	void finish(void);
	void update(void);
	void restore(void);
	void log_step(void);
	void predictor(void);
	void corrector(void);
	void clear_state(void);
	void update_state(void);

	//data
	double m_dl;
	double m_dl0;
	double m_ddl;
	double m_l_old;
	double m_l_new;

	uint32_t m_step;
	uint32_t m_step_max;
	uint32_t m_watch_dof;
	uint32_t m_iteration;
	uint32_t m_iteration_max;

	math::vector m_r;
	math::vector m_fn;
	math::vector m_fi;
	math::vector m_fe;

	math::matrix m_Kt;
	math::matrix m_Ct;
	math::matrix m_Mt;

	math::vector m_dx;
	math::vector m_dxt;
	math::vector m_ddxt;
	math::vector m_ddxr;

	bool m_log;
	bool m_save;
	bool m_equilibrium;
	double* m_state_old;
	double* m_state_new;
	double* m_state_data;
	double* m_cables_data;
	double* m_solver_data;
	double* m_energy_data;

	Strategy* m_strategy;
	Tensegrity* m_tensegrity;

	//friends
	friend class Deformed;
};