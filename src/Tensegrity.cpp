//std
#include <cmath>

//qt
#include <QtWidgets/QApplication>

//math
#include "Math/inc/misc/misc.hpp"

//canvas
#include "Canvas/inc/Scene/Scene.hpp"
#include "Canvas/inc/Objects/1D/Line.hpp"
#include "Canvas/inc/Objects/3D/Cube.hpp"
#include "Canvas/inc/Objects/3D/Cylinder.hpp"

//Tensegrity
#include "Tensegrity/inc/Model.hpp"
#include "Tensegrity/inc/Window.hpp"
#include "Tensegrity/inc/Solver.hpp"
#include "Tensegrity/inc/Deformed.hpp"
#include "Tensegrity/inc/Tensegrity.hpp"

//constructors
Tensegrity::Tensegrity(void) : m_nc(3), m_inelastic(false), m_solver(new Solver(this))
{
	//data
	m_rod_length = 5.00e-02;
	m_twist_angle = 0.00e+00;
	m_rod_thicknes = 1.00e-02;
	m_yield_stress = 4.00e+08;
	m_height_total = 3.20e-01;
	m_height_center = 1.40e-01;
	m_plate_diameter = 2.80e-01;
	m_plate_thickness = 5.00e-03;
	m_cables_diameter = 1.50e-03;
	m_residual_stress = 0.00e+00;
	m_elastic_modulus = 2.00e+11;
	//setup
	sprintf(m_label, "Tensegrity");
	m_strain_measure = Strain::strain_measure::quadratic;
}

//destructor
Tensegrity::~Tensegrity(void)
{
	delete m_solver;
}

//data
Solver* Tensegrity::solver(void) const
{
	return m_solver;
}

double Tensegrity::tension(double fr)
{
	const double dc = m_cables_diameter;
	const double sr = m_residual_stress;
	m_residual_stress = fr / (M_PI_4 * dc * dc);
	return fr;
}
double Tensegrity::tension(void) const
{
	const double dc = m_cables_diameter;
	const double sr = m_residual_stress;
	return sr * M_PI / 4 * dc * dc;
}

uint32_t Tensegrity::cables(void) const
{
	return m_nc;
}
uint32_t Tensegrity::cables(uint32_t nc)
{
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

const char* Tensegrity::label(void) const
{
	return m_label;
}
const char* Tensegrity::label(const char* label)
{
	sprintf(m_label, "%s", label);
	return m_label;
}

double Tensegrity::twist_angle(void) const
{
	return m_twist_angle;
}

double Tensegrity::rod_thickness(void) const
{
	return m_rod_thicknes;
}
double Tensegrity::rod_thickness(double rod_thickness)
{
	return m_rod_thicknes = rod_thickness;
}

double Tensegrity::twist_angle(double twist_angle)
{
	return m_twist_angle = twist_angle;
}

double Tensegrity::yield_stress(void) const
{
	return m_yield_stress;
}
double Tensegrity::yield_stress(double yield_stress)
{
	return m_yield_stress = yield_stress;
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

double Tensegrity::residual_stress(void) const
{
	return m_residual_stress;
}
double Tensegrity::residual_stress(double residual_stress)
{
	return m_residual_stress = residual_stress;
}

double Tensegrity::elastic_modulus(void) const
{
	return m_elastic_modulus;
}
double Tensegrity::elastic_modulus(double elastic_modulus)
{
	return m_elastic_modulus = elastic_modulus;
}

Strain::strain_measure Tensegrity::strain_measure(void) const
{
	return m_strain_measure;
}
Strain::strain_measure Tensegrity::strain_measure(Strain::strain_measure strain_measure)
{
	return m_strain_measure = strain_measure;
}

//draw
void Tensegrity::show(int32_t& argc, char** argv)
{
	//application
	QSurfaceFormat format;
	QApplication application(argc, argv);
	//surface
	format.setSamples(4);
	QSurfaceFormat::setDefaultFormat(format);
	//window
	Window window(this);
	window.showMaximized();
	//update
	window.model()->update();
	//execute
	application.exec();
}
void Tensegrity::draw_model(canvas::Scene* scene) const
{
	//data
	const float er =(float) m_rod_length;
	const float tr = (float) m_rod_thicknes;
	const float Ht = (float) m_height_total;
	const float Hc = (float) m_height_center;
	const float dp = (float) m_plate_diameter;
	const float tp = (float) m_plate_thickness;
	const float Hr = (Ht + Hc) / 2;
	canvas::objects::Cube* rod_1 = new canvas::objects::Cube;
	canvas::objects::Cube* rod_2 = new canvas::objects::Cube;
	canvas::objects::Cube* rod_3 = new canvas::objects::Cube;
	canvas::objects::Cube* rod_4 = new canvas::objects::Cube;
	canvas::objects::Cylinder* plate_1 = new canvas::objects::Cylinder;
	canvas::objects::Cylinder* plate_2 = new canvas::objects::Cylinder;
	//plates
	plate_1->height(tp);
	plate_2->height(tp);
	plate_1->radius(dp / 2);
	plate_2->radius(dp / 2);
	plate_2->shift({0, 0, Ht});
	plate_1->color_fill("blue");
	plate_2->color_fill("blue");
	//rods
	rod_1->color_fill("blue");
	rod_2->color_fill("blue");
	rod_3->color_fill("blue");
	rod_4->color_fill("blue");
	rod_2->scale({er, tr, tr});
	rod_4->scale({er, tr, tr});
	rod_2->shift({(er - tr) / 2, 0, Hr});
	rod_4->shift({(tr - er) / 2, 0, Ht - Hr});
	rod_1->scale({tr, tr, (2 * Hr + tr - tp) / 2});
	rod_3->scale({tr, tr, (2 * Hr + tr - tp) / 2});
	rod_1->shift({+er, 0, (2 * Hr + tr + tp) / 4});
	rod_3->shift({-er, 0, (2 * Hr + tr - tp) / 4 + Ht - Hr - tp});
	//cables
	scene->clear_objects(true);
	for(uint32_t k = 0; k < m_nc + 1; k++)
	{
		const float t = 2 * float(M_PI) * k / m_nc;
		canvas::objects::Line* cable = new canvas::objects::Line;
		cable->color_stroke("green");
		if(k == 0)
		{
			cable->point(1, {0, 0, Hr + tr / 2});
			cable->point(0, {0, 0, Ht - Hr + tr / 2});
		}
		else
		{
			cable->point(0, {dp / 2 * cosf(t), dp / 2 * sinf(t), tp / 2});
			cable->point(1, {dp / 2 * cosf(t), dp / 2 * sinf(t), Ht - tp / 2});
		}
		scene->add_object(cable);
	}
	//objects
	scene->add_object(rod_1);
	scene->add_object(rod_2);
	scene->add_object(rod_3);
	scene->add_object(rod_4);
	scene->add_object(plate_1);
	scene->add_object(plate_2);
}
void Tensegrity::draw_deformed(canvas::Scene* scene, uint32_t step) const
{
	//data
	const float er =(float) m_rod_length;
	const float tr = (float) m_rod_thicknes;
	const float Ht = (float) m_height_total;
	const float Hc = (float) m_height_center;
	const float dp = (float) m_plate_diameter;
	const float tp = (float) m_plate_thickness;
	const float Hr = (Ht + Hc) / 2;
	canvas::objects::Cube* rod_1 = new canvas::objects::Cube;
	canvas::objects::Cube* rod_2 = new canvas::objects::Cube;
	canvas::objects::Cube* rod_3 = new canvas::objects::Cube;
	canvas::objects::Cube* rod_4 = new canvas::objects::Cube;
	canvas::objects::Cylinder* plate_1 = new canvas::objects::Cylinder;
	canvas::objects::Cylinder* plate_2 = new canvas::objects::Cylinder;
	//state
	const math::vec3 z(0, 0, -Ht);
	const math::vec3 u(m_solver->m_state_data + 7 * step);
	const math::quat q(m_solver->m_state_data + 7 * step + 3);
	const canvas::vec3 cz(0, 0, Ht);
	const canvas::vec3 cu(u[0], u[1], u[2]);
	const canvas::quat cr(q[0], q[1], q[2], q[3]);
	//plates
	plate_1->height(tp);
	plate_2->height(tp);
	plate_1->radius(dp / 2);
	plate_2->radius(dp / 2);
	plate_2->shift({0, 0, Ht});
	plate_1->color_fill("blue");
	plate_2->color_fill("blue");
	//rods
	rod_1->color_fill("blue");
	rod_2->color_fill("blue");
	rod_3->color_fill("blue");
	rod_4->color_fill("blue");
	rod_2->scale({er, tr, tr});
	rod_4->scale({er, tr, tr});
	rod_2->shift({(er - tr) / 2, 0, Hr});
	rod_4->shift({(tr - er) / 2, 0, Ht - Hr});
	rod_1->scale({tr, tr, (2 * Hr + tr - tp) / 2});
	rod_3->scale({tr, tr, (2 * Hr + tr - tp) / 2});
	rod_1->shift({+er, 0, (2 * Hr + tr + tp) / 4});
	rod_3->shift({-er, 0, (2 * Hr + tr - tp) / 4 + Ht - Hr - tp});
	//cables
	scene->clear_objects(true);
	for(uint32_t k = 0; k < m_nc + 1; k++)
	{
		const float t = 2 * float(M_PI) * k / m_nc;
		canvas::objects::Line* cable = new canvas::objects::Line;
		cable->color_stroke("green");
		if(k == 0)
		{
			cable->point(1, {0, 0, Hr + tr / 2});
			cable->point(0, canvas::mat4::shifting(cu) * canvas::mat4::rotation(cz, cr) * canvas::vec3(0, 0, Ht - Hr + tr / 2));
		}
		else
		{
			cable->point(0, {dp / 2 * cosf(t), dp / 2 * sinf(t), tp / 2});
			cable->point(1, canvas::mat4::shifting(cu) * canvas::mat4::rotation(cz, cr) * canvas::vec3(dp / 2 * cosf(t), dp / 2 * sinf(t), Ht - tp / 2));
		}
		scene->add_object(cable);
	}
	//deformation
	rod_3->rotate(cz, cr);
	rod_4->rotate(cz, cr);
	plate_2->rotate(cz, cr);
	rod_3->shift(cu);
	rod_4->shift(cu);
	plate_2->shift(cu);
	//objects
	scene->add_object(rod_1);
	scene->add_object(rod_2);
	scene->add_object(rod_3);
	scene->add_object(rod_4);
	scene->add_object(plate_1);
	scene->add_object(plate_2);
}

//loads
void Tensegrity::remove_load(uint32_t index)
{
	m_pk.erase(m_pk.begin() + index);
	m_ak.erase(m_ak.begin() + index);
}
void Tensegrity::add_load(math::vec3 pk, math::vec3 ak)
{
	m_pk.push_back(pk);
	m_ak.push_back(ak);
}

void Tensegrity::load(uint32_t index, math::vec3 pk)
{
	m_pk[index] = pk;
}
void Tensegrity::load_position(uint32_t index, math::vec3 ak)
{
	m_ak[index] = ak;
}
const std::vector<math::vec3>& Tensegrity::loads(void) const
{
	return m_pk;
}
const std::vector<math::vec3>& Tensegrity::loads_position(void) const
{
	return m_ak;
}

//formulation
void Tensegrity::modes(void) const
{
	//data
	math::vector s(std::min(6U, m_nc + 1));
	const math::vec3 zr(0, 0, m_height_total);
	math::matrix A(6, m_nc + 1), U(6, 6), V(m_nc + 1, m_nc + 1);
	//assembly
	for(uint32_t k = 0; k < m_nc + 1; k++)
	{
		//position
		const math::vec3 z1 = position(k, 0, 0);
		const math::vec3 z2 = position(k, 1, 0);
		//direction
		const math::vec3 r = z2 - zr;
		const math::vec3 t = (z2 - z1).normalize();
		//assembly
		A.span(0, k, 3, 1) = t;
		A.span(3, k, 3, 1) = r.cross(t);
	}
	//decomposition
	A.svd(U, V, s);
	//print
	A.print("A");
	U.print("U");
	V.transpose().print("V");
	s.print("s");
}
double Tensegrity::cable_force(uint32_t index) const
{
	//data
	const double sr = m_residual_stress;
	const double Ec = m_elastic_modulus;
	const double dc = m_cables_diameter;
	const double Ac = M_PI * dc * dc / 4;
	//position
	const math::vec3 z1 = position(index, 0, 0);
	const math::vec3 z2 = position(index, 1, 0);
	const math::vec3 x1 = position(index, 0, 1);
	const math::vec3 x2 = position(index, 1, 1);
	//lengths
	const double Lk = (z2 - z1).norm();
	const double lk = (x2 - x1).norm();
	//strain
	const double ak = lk / Lk;
	const double ek = Strain::strain(ak, m_strain_measure);
	const double gk = Strain::strain_gradient(ak, m_strain_measure);
	//return
	const uint32_t nk = index == 0 ? m_nc : 1;
	return fmax(Ec * Ac * ek * gk + nk * sr * Ac, 0);
}

double Tensegrity::internal_energy(void) const
{
	//data
	double U = 0;
	const double sr = m_residual_stress;
	const double Ec = m_elastic_modulus;
	const double dc = m_cables_diameter;
	const double Ac = M_PI * dc * dc / 4;
	//internal energy
	for(uint32_t k = 0; k <= m_nc; k++)
	{
		//position
		const math::vec3 z1 = position(k, 0, 0);
		const math::vec3 z2 = position(k, 1, 0);
		const math::vec3 x1 = position(k, 0, 1);
		const math::vec3 x2 = position(k, 1, 1);
		//lengths
		const double Lk = (z2 - z1).norm();
		const double lk = (x2 - x1).norm();
		//strain
		const double ak = lk / Lk;
		const double ek = Strain::strain(ak, m_strain_measure);
		const double gk = Strain::strain_gradient(ak, m_strain_measure);
		//energy
		const uint32_t nk = k == 0 ? m_nc : 1;
		const double fk = fmax(Ec * Ac * ek * gk + sr * Ac, 0);
		U += bool(fk) * Ac * Lk * (Ec / 2 * ek * ek + nk * ak * sr);
	}
	//return
	return U;
}
double Tensegrity::potential_energy(void) const
{
	//data
	double V = 0;
	const math::vec3 zr(0, 0, m_height_total);
	const math::vec3 qr(m_solver->m_state_new + 3);
	//potential energy
	for(uint32_t i = 0; i < m_pk.size(); i++)
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
	const double sr = m_residual_stress;
	const double Ec = m_elastic_modulus;
	const double dc = m_cables_diameter;
	const double Ac = M_PI * dc * dc / 4;
	const math::mat3 I = math::mat3::eye();
	const math::vec3 zr(0, 0, m_height_total);
	const math::quat qr(m_solver->m_state_new + 3);
	//internal force
	for(uint32_t k = 0; k <= m_nc; k++)
	{
		//position
		const math::vec3 z1 = position(k, 0, 0);
		const math::vec3 z2 = position(k, 1, 0);
		const math::vec3 x1 = position(k, 0, 1);
		const math::vec3 x2 = position(k, 1, 1);
		const math::vec3 rk = qr.rotate(z2 - zr);
		const math::vec3 tk = (x2 - x1).normalize();
		//lengths
		const double Lk = (z2 - z1).norm();
		const double lk = (x2 - x1).norm();
		//strain
		const double ak = lk / Lk;
		const double ek = Strain::strain(ak, m_strain_measure);
		const double hk = Strain::strain_hessian(ak, m_strain_measure);
		const double gk = Strain::strain_gradient(ak, m_strain_measure);
		//force
		const uint32_t nk = k == 0 ? m_nc : 1;
		const double fk = fmax(Ec * Ac * ek * gk + nk * Ac * sr, 0);
		const double Kk = bool(fk) * Ec * Ac / Lk * (gk * gk + ek * hk);
		const math::mat3 Ak = (Kk - fk / lk) * tk.outer(tk) + fk / lk * I;
		//stiffness
		K.span(0, 0) += Ak;
		K.span(0, 3) -= Ak * rk.spin();
		K.span(3, 0) += rk.spin() * Ak;
		K.span(3, 3) += fk * tk.spin() * rk.spin();
		K.span(3, 3) -= rk.spin() * Ak * rk.spin();
	}
	//external force
	for(uint32_t k = 0; k < m_pk.size(); k++)
	{
		//force
		const math::vec3 ck = qr.rotate(m_ak[k] - zr);
		const math::vec3 pk = m_solver->m_l_new * m_pk[k];
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
	const double sr = m_residual_stress;
	const double Ec = m_elastic_modulus;
	const double dc = m_cables_diameter;
	const double Ac = M_PI * dc * dc / 4;
	const math::vec3 zr(0, 0, m_height_total);
	const math::quat qr(m_solver->m_state_new + 3);
	//internal force
	for(uint32_t k = 0; k <= m_nc; k++)
	{
		//position
		const math::vec3 z1 = position(k, 0, 0);
		const math::vec3 z2 = position(k, 1, 0);
		const math::vec3 x1 = position(k, 0, 1);
		const math::vec3 x2 = position(k, 1, 1);
		const math::vec3 tk = (x2 - x1).normalize();
		//lengths
		const double Lk = (z2 - z1).norm();
		const double lk = (x2 - x1).norm();
		//strain
		const double ak = lk / Lk;
		const double ek = Strain::strain(ak, m_strain_measure);
		const double gk = Strain::strain_gradient(ak, m_strain_measure);
		//axial force
		const uint32_t nk = k == 0 ? m_nc : 1;
		const double fk = fmax(Ec * Ac * ek * gk + nk * sr * Ac, 0);
		//internal force
		fi.span(0, 0, 3, 1) += fk * tk;
		fi.span(3, 0, 3, 1) += fk * qr.rotate(z2 - zr).cross(tk);
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
	for(uint32_t i = 0; i < m_pk.size(); i++)
	{
		math::vec3(fe.data() + 0) += m_pk[i];
		math::vec3(fe.data() + 3) += qr.rotate(m_ak[i] - zr).cross(m_pk[i]);
	}
}

//operators
Tensegrity& Tensegrity::operator=(const Tensegrity& tensegrity)
{
	m_nc = tensegrity.m_nc;
	*m_solver = *tensegrity.m_solver;
	strcpy(m_label, tensegrity.m_label);
	m_inelastic = tensegrity.m_inelastic;
	m_rod_length = tensegrity.m_rod_length;
	m_twist_angle = tensegrity.m_twist_angle;
	m_yield_stress = tensegrity.m_yield_stress;
	m_height_total = tensegrity.m_height_total;
	m_height_center = tensegrity.m_height_center;
	m_strain_measure = tensegrity.m_strain_measure;
	m_plate_diameter = tensegrity.m_plate_diameter;
	m_plate_thickness = tensegrity.m_plate_thickness;
	m_cables_diameter = tensegrity.m_cables_diameter;
	m_residual_stress = tensegrity.m_residual_stress;
	m_elastic_modulus = tensegrity.m_elastic_modulus;
	return *this;
}

//position
math::vec3 Tensegrity::position(uint32_t index, bool level, bool configuration) const
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
			(index != 0) * level * Ht + (index == 0) * (Hr - level * Hc)
		};
	}
	else
	{
		const math::vec3 zr(0, 0, Ht);
		const math::vec3 ur(m_solver->m_state_new + 0);
		const math::quat qr(m_solver->m_state_new + 3);
		return zr + ur + qr.rotate(position(index, true, false) - zr);
	}
}