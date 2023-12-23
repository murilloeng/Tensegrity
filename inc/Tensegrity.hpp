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
	math::matrix stiffness(void) const;
	math::vector internal_force(void) const;

private:
	//draw
	void draw_model_disks(canvas::Scene*) const;
	void draw_model_links(canvas::Scene*) const;
	void draw_model_latex(canvas::Scene*) const;
	void draw_model_cables(canvas::Scene*) const;
	void draw_model_guides(canvas::Scene*) const;

public:
	//data
	double m_b0;
	double m_er;
	double m_tl;
	double m_tr;
	double m_Rr;
	double m_Ht;
	double m_Hc;
	double m_Ec;
	double m_dc;
	unsigned m_nc;
	math::vec3 m_x;
	math::quat m_q;
};