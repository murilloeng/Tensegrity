#pragma once

//std
#include <functional>

//math
#include "Math/inc/linear/vec3.hpp"
#include "Math/inc/linear/quat.hpp"
#include "Math/inc/linear/mat3.hpp"

//canvas
#include "Canvas/lib/inc/Scene/Scene.hpp"

class Tensegrity
{
public:
	//constructors
	Tensegrity(void);

	//destructor
	~Tensegrity(void);

	//draw
	void draw_model(canvas::Scene*) const;

	//solver
	void solve(void);

private:
	//solver
	void setup(void);
	void record(void);
	void update(void);
	void restore(void);
	void update_state(void);
	void solve_static(void);
	void solve_dynamic(void);

	//compute
	void compute_mass(void);
	void compute_center(void);
	void compute_energy(void);
	void compute_inertia(void);
	void compute_acceleration(void);
	void compute_load_predictor(void);
	void compute_load_corrector(void);
	
	//formulation
	void inertia(math::matrix&) const;
	void damping(math::matrix&) const;
	void stiffness(math::matrix&) const;
	void inertial_force(math::vector&) const;
	void internal_force(math::vector&) const;
	void external_force(math::vector&) const;
	
	//cables
	double cable_area(void) const;
	double cable_force(unsigned) const;
	double cable_energy(unsigned) const;
	double cable_length(unsigned) const;
	double cable_stretch(unsigned) const;
	double cable_stiffness(unsigned) const;
	double cable_strain_measure(unsigned) const;
	double cable_strain_hessian(unsigned) const;
	double cable_strain_gradient(unsigned) const;

	//draw
	void draw_model_disks(canvas::Scene*) const;
	void draw_model_links(canvas::Scene*) const;
	void draw_model_latex(canvas::Scene*) const;
	void draw_model_cables(canvas::Scene*) const;
	void draw_model_guides(canvas::Scene*) const;

	//position
	math::vec3 position(unsigned, bool, bool) const;

public:
	//data
	double m_T;
	double m_pr;
	double m_er;
	double m_tl;
	double m_tr;
	double m_ar;
	double m_br;
	double m_Rr;
	double m_Ht;
	double m_Hc;
	double m_Ec;
	double m_dc;
	unsigned m_nc;
	unsigned m_type;
	unsigned m_step;
	unsigned m_steps;
	unsigned m_solver;
	unsigned m_iteration;
	unsigned m_iteration_max;

	double* m_state_data;
	double* m_energy_data;
	double* m_velocity_data;
	double* m_acceleration_data;

	double m_M;
	double m_dl;
	double m_ddl;
	double m_l_old;
	double m_l_new;
	math::mat3 m_J;
	math::vec3 m_zc;
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
	double m_state_old[7];
	double m_state_new[7];
	double m_velocity_old[6];
	double m_velocity_new[6];
	double m_acceleration_old[6];
	double m_acceleration_new[6];
	std::vector<math::vec3> m_ak;
	std::vector<std::function<math::vec3(double)>> m_pk;
};