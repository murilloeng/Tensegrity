#pragma once

class Chair
{
public:
	//constructors
	Chair(void);

	//destructor
	~Chair(void);

	//geometry
	double Hr(void) const;
	double Ac(void) const;

	//strain
	double stretch(unsigned) const;
	double strain_measure(unsigned) const;
	double strain_hessian(unsigned) const;
	double strain_gradient(unsigned) const;

	//position
	void position(double*, unsigned, bool, bool) const;

	//formulation
	void stiffness(double*) const;
	void internal_force(double*) const;

	//data
	double m_e;
	double m_t0;
	double m_Rr;
	double m_Ht;
	double m_Hc;
	double m_Ec;
	double m_dc;
	double m_x[3];
	double m_t[3];
	unsigned m_nc;
};