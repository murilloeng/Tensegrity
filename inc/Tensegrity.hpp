#pragma once

//math
#include "Math/inc/linear/vec3.hpp"
#include "Math/inc/linear/quat.hpp"

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

	//cables
	double cable_stretch(unsigned) const;
	double cable_strain_measure(unsigned) const;
	double cable_strain_hessian(unsigned) const;
	double cable_strain_gradient(unsigned) const;

	//position
	math::vec3 position(unsigned, bool, bool) const;

	//formulation
	math::vector residue(void) const;
	math::matrix inertia(void) const;
	math::matrix damping(void) const;
	math::matrix stiffness(void) const;

private:
	//draw
	void draw_model_disks(canvas::Scene*) const;
	void draw_model_links(canvas::Scene*) const;
	void draw_model_latex(canvas::Scene*) const;
	void draw_model_cables(canvas::Scene*) const;
	void draw_model_guides(canvas::Scene*) const;

public:
	//data
	double m_er;
	double m_tl;
	double m_tr;
	double m_Rr;
	double m_Ht;
	double m_Hc;
	double m_Ec;
	double m_dc;
	unsigned m_nc;

	double* m_state_data;
	double* m_velocity_data;
	double* m_acceleration_data;

	double m_state_old[7];
	double m_state_new[7];
	double* m_velocity_old[6];
	double* m_velocity_new[6];
	double* m_acceleration_old[6];
	double* m_acceleration_new[6];
	
};