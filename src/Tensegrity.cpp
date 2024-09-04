//std
#include <cmath>

//math
#include "Math/inc/misc/misc.hpp"

//Tensegrity
#include "Tensegrity/inc/Solver.hpp"
#include "Tensegrity/inc/Tensegrity.hpp"

//constructors
Tensegrity::Tensegrity(void) : m_nc(3), m_inelastic(false), m_solver(new Solver(this))
{
	m_Ec = new double[m_nc];
	m_sr = new double[m_nc];
	m_sy = new double[m_nc];
	m_rod_length = 5.00e-02;
	m_twist_angle = 0.00e+00;
	m_height_total = 3.20e-01;
	m_height_center = 1.40e-01;
	m_plate_diameter = 2.80e-01;
	m_plate_thickness = 5.00e-03;
	m_cables_diameter = 1.50e-03;
	sprintf(m_label, "Tensegrity");
}

//destructor
Tensegrity::~Tensegrity(void)
{
	delete[] m_Ec;
	delete[] m_sr;
	delete[] m_sy;
	delete m_solver;
}

//data
uint32_t Tensegrity::cables(void) const
{
	return m_nc;
}
uint32_t Tensegrity::cables(uint32_t nc)
{
	delete[] m_Ec;
	delete[] m_sr;
	delete[] m_sy;
	m_Ec = new double[nc];
	m_sr = new double[nc];
	m_sy = new double[nc];
	return m_nc = nc;
}

double Tensegrity::rod_length(void) const
{
	return m_rod_length;
}
double Tensegrity::rod_length(double rod_length)
{
	return m_rod_length = rod_length;
}

double Tensegrity::twist_angle(void) const
{
	return m_twist_angle;
}
double Tensegrity::twist_angle(double twist_angle)
{
	return m_twist_angle = twist_angle;
}

double Tensegrity::height_total(void) const
{
	return m_height_total;
}
double Tensegrity::height_total(double height_total)
{
	return m_height_total = height_total;
}

double Tensegrity::height_center(void) const
{
	return m_height_center;
}
double Tensegrity::height_center(double height_center)
{
	return m_height_center = height_center;
}

double Tensegrity::plate_diameter(void) const
{
	return m_plate_diameter;
}
double Tensegrity::plate_diameter(double plate_diameter)
{
	return m_plate_diameter = plate_diameter;
}

double Tensegrity::plate_thickness(void) const
{
	return m_plate_thickness;
}
double Tensegrity::plate_thickness(double plate_thickness)
{
	return m_plate_thickness = plate_thickness;
}

double Tensegrity::cables_diameter(void) const
{
	return m_cables_diameter;
}
double Tensegrity::cables_diameter(double cables_diameter)
{
	return m_cables_diameter = cables_diameter;
}

const double* Tensegrity::yield_stress(double sy)
{
	for(uint32_t i = 0; i < m_nc; i++)
	{
		m_sy[i] = sy;
	}
	return m_sy;
}
const double* Tensegrity::yield_stress(void) const
{
	return m_sy;
}
const double* Tensegrity::yield_stress(const double* sy)
{
	return (const double*) memcpy(m_sy, sy, m_nc * sizeof(double));
}

const double* Tensegrity::residual_stress(double sr)
{
	for(uint32_t i = 0; i < m_nc; i++)
	{
		m_sr[i] = sr;
	}
	return m_sr;
}
const double* Tensegrity::residual_stress(void) const
{
	return m_sr;
}
const double* Tensegrity::residual_stress(const double* sr)
{
	return (const double*) memcpy(m_sr, sr, m_nc * sizeof(double));
}

const double* Tensegrity::elastic_modulus(double Ec)
{
	for(uint32_t i = 0; i < m_nc; i++)
	{
		m_Ec[i] = Ec;
	}
	return m_Ec;
}
const double* Tensegrity::elastic_modulus(void) const
{
	return m_Ec;
}
const double* Tensegrity::elastic_modulus(const double* Ec)
{
	return (const double*) memcpy(m_Ec, Ec, m_nc * sizeof(double));
}

//formulation
double Tensegrity::internal_energy(void) const
{
	//data
	double U = 0;
	const double dc = m_cables_diameter;
	const double Ac = M_PI * dc * dc / 4;
	//internal energy
	for(unsigned i = 0; i <= m_nc; i++)
	{
		//position
		const math::vec3 z1 = position(i, 0, 0);
		const math::vec3 z2 = position(i, 1, 0);
		const math::vec3 x1 = position(i, 0, 1);
		const math::vec3 x2 = position(i, 1, 1);
		//lengths
		const double Lk = (z2 - z1).norm();
		const double lk = (x2 - x1).norm();
		//stretch
		const double ak = lk / Lk;
		//strain
		const double ek = log(ak);
		//energy
		U += m_Ec[i] * Ac * Lk / 2 * ek * ek * (ek > 0);
	}
	//return
	return U;
}
double Tensegrity::potential_energy(void) const
{
	//data
	double V = 0;
	const double T = m_solver->m_T;
	const double Ht = m_height_total;
	const unsigned s = m_solver->m_step;
	const unsigned ns = m_solver->m_step_max;
	const double t = m_solver->m_type * s * T / ns;
	const math::vec3 qr(m_solver->m_state_new + 3);
	//potential energy
	const math::vec3 zr(0, 0, Ht);
	for(unsigned i = 0; i < m_pk.size(); i++)
	{
		V -= qr.rotate(m_ak[i] - zr).inner(m_pk[i]);
	}
	//return
	return V;
}

void Tensegrity::stiffness(math::matrix& K) const
{
	//data
	K.zeros();
	const double Ht = m_height_total;
	const double dc = m_cables_diameter;
	const math::vec3 zr(0, 0, Ht);
	const math::quat qr(m_solver->m_state_new + 3);
	const double A = M_PI * dc * dc / 4;
	//internal force
	for(unsigned i = 0; i <= m_nc; i++)
	{
		//position
		const math::vec3 z1 = position(i, 0, 0);
		const math::vec3 z2 = position(i, 1, 0);
		const math::vec3 x1 = position(i, 0, 1);
		const math::vec3 x2 = position(i, 1, 1);
		const math::vec3 rk = qr.rotate(z2 - zr);
		const math::vec3 tk = (x2 - x1).normalize();
		//lengths
		const double Lk = (z2 - z1).norm();
		const double lk = (x2 - x1).norm();
		//stretch
		const double ak = lk / Lk;
		//strain
		const double gk = 1 / ak;
		const double ek = log(ak);
		const double hk = -1 / ak / ak;
		//stress
		const double sk = fmax(m_Ec[i] * ek, 0);
		const double Ck = m_Ec[i] * (m_Ec[i] * ek >= 0);
		//force
		const double fk = sk * A * gk;
		const double Kk = A / Lk * (Ck * gk * gk + sk * hk);
		//matrices
		const math::mat3 Ak = tk.outer(tk);
		const math::mat3 Bk = math::mat3::eye() - Ak;
		//stiffness
		K.span(0, 0) += Kk * Ak + fk / lk * Bk;
		K.span(3, 3) += fk * tk.spin() * rk.spin();
		K.span(0, 3) -= (Kk * Ak + fk / lk * Bk) * rk.spin();
		K.span(3, 0) += rk.spin() * (Kk * Ak + fk / lk * Bk);
		K.span(3, 3) -= rk.spin() * (Kk * Ak + fk / lk * Bk) * rk.spin();
	}
	//external force
	for(unsigned i = 0; i < m_pk.size(); i++)
	{
		//force
		const math::vec3 ck = qr.rotate(m_ak[i] - zr);
		const math::vec3 pk = m_solver->m_l_new * m_pk[i];
		//stiffness
		K.span(3, 3) -= pk.spin() * ck.spin();
	}
	//parametrization
	const math::quat qr_old(m_solver->m_state_old + 3);
	const math::vec3 tr_inc = qr_old.conjugate(qr).pseudo();
	K.span(0, 3, 6, 3) = K.span(0, 3, 6, 3) * qr_old.rotation() * tr_inc.rotation_gradient();
}

void Tensegrity::internal_force(math::vector& fi) const
{
	//data
	fi.zeros();
	const double Ht = m_height_total;
	const double dc = m_cables_diameter;
	const math::vec3 zr(0, 0, Ht);
	const math::quat qr(m_solver->m_state_new + 3);
	const double A = M_PI * dc * dc / 4;
	//internal force
	for(unsigned i = 0; i <= m_nc; i++)
	{
		//position
		const math::vec3 z1 = position(i, 0, 0);
		const math::vec3 z2 = position(i, 1, 0);
		const math::vec3 x1 = position(i, 0, 1);
		const math::vec3 x2 = position(i, 1, 1);
		const math::vec3 tk = (x2 - x1).normalize();
		//lengths
		const double Lk = (z2 - z1).norm();
		const double lk = (x2 - x1).norm();
		//stretch
		const double ak = lk / Lk;
		//strain
		const double gk = 1 / ak;
		const double ek = log(ak);
		//stress
		const double se = m_Ec[i] * ek;
		const double sc = se > 0 ? se : 0;
		const double sk = sc;
		//axial force
		const double fk = sk * A * gk;
		//internal force
		fi.span(0, 0, 3, 1) += fk * tk;
		fi.span(3, 0, 3, 1) += qr.rotate(z2 - zr).cross(fk * tk);
	}
}
void Tensegrity::external_force(math::vector& fe) const
{
	//data
	fe.zeros();
	const double Ht = m_height_total;
	const math::quat qr(m_solver->m_state_new + 3);
	//external force
	const math::vec3 zr(0, 0, Ht);
	for(unsigned i = 0; i < m_pk.size(); i++)
	{
		math::vec3(fe.data() + 0) += m_pk[i];
		math::vec3(fe.data() + 3) += qr.rotate(m_ak[i] - zr).cross(m_pk[i]);
	}
}

//position
math::vec3 Tensegrity::position(unsigned index, bool level, bool configuration) const
{
	//data
	const double q0 = m_twist_angle;
	const double Ht = m_height_total;
	const double Hc = m_height_center;
	const double Rp = m_plate_diameter / 2;
	//position
	if(!level || !configuration)
	{
		const double Hr = (Ht + Hc) / 2;
		return {
			(index != 0) * Rp * cos(2 * M_PI * index / m_nc + q0 * level),
			(index != 0) * Rp * sin(2 * M_PI * index / m_nc + q0 * level),
			(index == 0) * (Hr - level * Hc) + (index != 0) * level * Ht
		};
	}
	else
	{
		const math::vec3 zr(0, 0, Ht);
		const math::vec3 ur(m_solver->m_state_new + 0);
		const math::quat qr(m_solver->m_state_new + 3);
		return zr + ur + qr.rotate(position(index, level, 0) - zr);
	}
}