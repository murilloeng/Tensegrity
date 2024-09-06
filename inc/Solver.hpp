#pragma once

//math
#include "Math/inc/linear/vector.hpp"

class Tensegrity;

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
	uint32_t step_max(uint32_t);
	uint32_t step_max(void) const;

	uint32_t iteration_max(uint32_t);
	uint32_t iteration_max(void) const;

	double load_predictor(double);
	double load_predictor(void) const;

	const double* state(void) const;
	const double* state(const double*);

	//solve
	void solve(void);

private:
	
	//solver
	void setup(void);
	void record(void);
	void finish(void);
	void update(void);
	void restore(void);
	void clear_state(void);
	void update_state(void);

	//formulation
	void compute_residue(void);
	void compute_load_predictor(void);
	void compute_load_corrector(void);

	//data
	double m_dl;
	double m_ddl;
	double m_l_old;
	double m_l_new;

	uint32_t m_step;
	uint32_t m_step_max;
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

	bool m_equilibrium;
	double* m_state_old;
	double* m_state_new;
	double* m_state_data;
	double* m_cables_data;
	double* m_solver_data;
	double* m_energy_data;

	Tensegrity* m_tensegrity;
};