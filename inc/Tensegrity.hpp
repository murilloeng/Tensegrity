#pragma once

//std
#include <functional>

//math
#include "Math/inc/linear/vec3.hpp"
#include "Math/inc/linear/quat.hpp"
#include "Math/inc/linear/mat3.hpp"

//canvas
#include "Canvas/inc/Scene/Scene.hpp"

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
	void draw_configuration(canvas::Scene*) const;

	//formulation
	void inertia(math::matrix&) const;
	void damping(math::matrix&) const;
	void stiffness(math::matrix&) const;
	
	double kinetic_energy(void) const;
	double internal_energy(void) const;
	double potential_energy(void) const;
	
	void inertial_force(math::vector&) const;
	void internal_force(math::vector&) const;
	void external_force(math::vector&) const;

	//friends
	friend class Solver;

private:
	//compute
	void compute_energy(void);
	void compute_inertia(void);
	void compute_acceleration(void);

	//draw
	void draw_disks(canvas::Scene*, bool = false) const;
	void draw_links(canvas::Scene*, bool = false) const;
	void draw_latex(canvas::Scene*, bool = false) const;
	void draw_cables(canvas::Scene*, bool = false) const;
	void draw_guides(canvas::Scene*, bool = false) const;

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
	double m_q0;
	double m_sr;
	unsigned m_nc;
	unsigned m_type;
	char m_label[200];

	Solver* m_solver;

	double m_M;
	double m_K0[6];
	math::mat3 m_J1;
	math::mat3 m_J2;
	math::vec3 m_zc;
	std::vector<math::vec3> m_ak;
	std::vector<math::vec3> m_pk;
	std::vector<math::vec3> m_mk;
};