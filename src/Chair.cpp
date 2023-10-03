//std
#include <cmath>

//math
#include "../Math/inc/linear/vec3.hpp"

//Tensegrity
#include "inc/Chair.hpp"

//constructors
Chair::Chair(void) : 
	m_e(5.00e-01), m_t0(0.00e+00), m_Rr(1.00e+00), m_Ht(1.00e+00), m_Hc(5.00e-01), m_Ec(8.00e+10), m_dc(1.00e-03), m_nc(3)
{
	return;
}

//destructor
Chair::~Chair(void)
{
	return;
}

//geometry
double Chair::Hr(void) const
{
	return (m_Ht + m_Hc) / 2;
}
double Chair::Ac(void) const
{
	return M_PI * m_dc * m_dc / 4;
}

//strain
double Chair::stretch(unsigned index) const
{
	return 0;
}
double Chair::strain_measure(unsigned) const
{
	return 0;
}
double Chair::strain_hessian(unsigned) const
{
	return 0;
}
double Chair::strain_gradient(unsigned) const
{
	return 0;
}

//position
void Chair::position(double* position, unsigned index, bool level, bool configuration) const
{
	//data
	const double t = 2 * M_PI * index / m_nc;
	//position
	if(!level || !configuration)
	{
		position[0] = (index != 0) * m_Rr * cos(t);
		position[1] = (index != 0) * m_Rr * sin(t);
		position[2] = (index == 0) * (Hr() - level * m_Hc) + (index != 0) * level * m_Ht;
	}
	else
	{
		math::vec3 x(position);
		this->position(position, index, true, false);
		x = math::vec3(m_x) + math::vec3(m_t).rotate(x - math::vec3(0, 0, m_Ht));
	}
}

//formulation
void Chair::stiffness(double*) const
{

}
void Chair::internal_force(double*) const
{

}