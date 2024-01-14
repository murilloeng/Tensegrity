#pragma once

//std
#include <functional>

//math
#include "Math/inc/linear/vec3.hpp"
#include "Math/inc/linear/quat.hpp"
#include "Math/inc/linear/mat3.hpp"

//canvas
#include "Canvas/lib/inc/Scene/Scene.hpp"

class Solver;

class Tensegrity
{
public:
	//constructors
	Tensegrity(void);

	//destructor
	~Tensegrity(void);

	//draw
	void draw_model(canvas::Scene*) const;

	//friends
	friend class Solver;

private:
	//compute
	void compute_mass(void);
	void compute_center(void);
	void compute_energy(void);
	void compute_inertia(void);
	void compute_acceleration(void);
	
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
	double m_a0;
	unsigned m_nc;
	unsigned m_type;

	Solver* m_solver;

	double m_M;
	math::mat3 m_J;
	math::vec3 m_zc;
	std::vector<math::vec3> m_ak;
	std::vector<std::function<math::vec3(double)>> m_pk;
};