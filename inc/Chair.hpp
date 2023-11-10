#pragma once

#include "inc/linear/vec3.hpp"

class Chair
{
public:
	//constructors
	Chair(void);

	//destructor
	~Chair(void);

	//strain
	double stretch(unsigned) const;
	double strain_measure(unsigned) const;
	double strain_hessian(unsigned) const;
	double strain_gradient(unsigned) const;

	//position
	math::vec3 position(unsigned, bool, bool) const;

	//formulation
	math::matrix stiffness(void) const;
	math::vector internal_force(void) const;

	//data
	double m_e;
	double m_t0;
	double m_Rr;
	double m_Ht;
	double m_Hc;
	double m_Ec;
	double m_dc;
	unsigned m_nc;
	math::vec3 m_x;
	math::vec3 m_t;
};