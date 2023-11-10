//std
#include <cmath>

//math
#include "../Math/inc/linear/vec3.hpp"

//Tensegrity
#include "inc/Chair.hpp"

//constructors
Chair::Chair(void) : 
	m_e(2.00e-01), m_t0(0.00e+00), m_Rr(4.00e-01), m_Ht(5.00e-01), m_Hc(2.50e-01), m_Ec(8.00e+10), m_dc(1.00e-03), m_nc(3)
{
	return;
}

//destructor
Chair::~Chair(void)
{
	return;
}

//strain
double Chair::stretch(unsigned index) const
{
	const math::vec3 z1 = position(index, 0, 0);
	const math::vec3 z2 = position(index, 1, 0);
	const math::vec3 x1 = position(index, 0, 1);
	const math::vec3 x2 = position(index, 1, 1);
	return (x2 - x1).norm() / (z2 - z1).norm();
}
double Chair::strain_measure(unsigned index) const
{
	return log(stretch(index));
}
double Chair::strain_hessian(unsigned index) const
{
	return -1 / pow(stretch(index), 2);
}
double Chair::strain_gradient(unsigned index) const
{
	return 1 / stretch(index);
}

//position
math::vec3 Chair::position(unsigned index, bool level, bool configuration) const
{
	if(!level || !configuration)
	{
		math::vec3 x;
		const double Hr = (m_Ht + m_Hc) / 2;
		x[2] = (index == 0) * (Hr - level * m_Hc) + (index != 0) * level * m_Ht;
		x[0] = (index != 0) * m_Rr * cos(2 * M_PI * index / m_nc + level * m_t0);
		x[1] = (index != 0) * m_Rr * sin(2 * M_PI * index / m_nc + level * m_t0);
		return x;
	}
	else
	{
		return m_x + m_t.rotate(position(index, 1, 0) - math::vec3(0, 0, m_Ht));
	}
}

//formulation
math::matrix Chair::stiffness(void) const
{
	//data
	math::matrix K(6, 6, math::mode::zeros);
	//stiffness

	//return
	return K;
}
math::vector Chair::internal_force(void) const
{
	//data
	math::vector fi(6, math::mode::zeros);
	const double Ac = M_PI * m_dc * m_dc / 4;
	//internal force
	for(unsigned i = 0; i < m_nc; i++)
	{
		//position
		const math::vec3 z1 = position(i, 0, 0);
		const math::vec3 z2 = position(i, 1, 0);
		const math::vec3 x1 = position(i, 0, 1);
		const math::vec3 x2 = position(i, 1, 1);
		//lenght
		const double L = (z2 - z1).norm();
		const double l = (x2 - x1).norm();
		const math::vec3 tc = (x2 - x1) / l;
		//strain
		const double a = l / L;
		const double e = log(a);
		const double de = 1 / a;
		const double he = -1 / a / a;
		//stress
		const double s = fmax(0, m_Ec * e);
		//internal force
		math::vec3(fi.data() + 0) += s * Ac * de * tc;
		math::vec3(fi.data() + 3) += s * Ac * de * (x2 - m_x).cross(tc);
	}
	//return
	return fi;
}