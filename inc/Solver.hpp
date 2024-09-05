#pragma once

//math
#include "Math/inc/linear/vector.hpp"

class Tensegrity;

class Solver
{
public:
	//constructors
	Solver(Tensegrity*);

	//destructor
	~Solver(void);

	//solve
	void solve(void);

	//friends
	friend class Tensegrity;

	//data
	bool m_log;
	double m_T;
	double m_dl;
	uint32_t m_type;
	uint32_t m_step_max;
	uint32_t m_iteration_max;

	double m_state_old[7];
	double m_state_new[7];

public:
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

public:
	//data
	double m_ddl;
	double m_l_old;
	double m_l_new;
	uint32_t m_step;
	uint32_t m_iteration;

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

	Tensegrity* m_tensegrity;

	bool m_equilibrium;
	double* m_state_data;
	double* m_cables_data;
	double* m_solver_data;
	double* m_energy_data;
};