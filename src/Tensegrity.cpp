//std
#include <cmath>

//math
#include "Math/inc/misc/misc.hpp"

//canvas
#include "Canvas/lib/inc/Objects/1D/Line.hpp"
#include "Canvas/lib/inc/Objects/1D/Arrow.hpp"

#include "Canvas/lib/inc/Objects/3D/Cube.hpp"
#include "Canvas/lib/inc/Objects/3D/Cylinder.hpp"

#include "Canvas/lib/inc/Objects/Image/Latex.hpp"

//Tensegrity
#include "inc/Solver.hpp"
#include "inc/Tensegrity.hpp"

//constructors
Tensegrity::Tensegrity(void) : 
	m_solver(new Solver(this)), m_a0(1.00e-05), 
	m_br(2.00e-01), m_Rr(2.00e-01), m_Ht(5.00e-01), m_Hc(2.50e-01), m_Ec(8.00e+10), m_dc(1.00e-03), 
	m_nc(3), m_type(0), m_pr(4.00e+02), m_er(1.00e-01), m_tl(1.00e-02), m_tr(1.00e-02), m_ar(2.00e-01)
{
	sprintf(m_label, "Tensegrity");
}

//destructor
Tensegrity::~Tensegrity(void)
{
	delete m_solver;
}

//draw
void Tensegrity::draw_model(canvas::Scene* scene) const
{
	draw_model_disks(scene);
	draw_model_links(scene);
	draw_model_latex(scene);
	draw_model_cables(scene);
	draw_model_guides(scene);
	scene->background({1, 1, 1, 1});
	scene->camera().rotation({M_PI_2, 0, 0});
}

//compute
void Tensegrity::compute_mass(void)
{
	//data
	const double Hr = (m_Ht + m_Hc) / 2;
	const double Ar = m_type ? m_ar * m_br : M_PI * m_Rr * m_Rr;
	//mass
	const double M1 = m_pr * Ar * m_tr;
	const double M2 = m_pr * Hr * m_tl * m_tl;
	const double M3 = m_pr * m_er * m_tl * m_tl;
	m_M = M1 + M2 + M3;
}
void Tensegrity::compute_center(void)
{
	//data
	const double Hr = (m_Ht + m_Hc) / 2;
	const double Ar = m_type ? m_ar * m_br : M_PI * m_Rr * m_Rr;
	//mass
	const double M1 = m_pr * Ar * m_tr;
	const double M2 = m_pr * Hr * m_tl * m_tl;
	const double M3 = m_pr * m_er * m_tl * m_tl;
	//center
	const math::vec3 z1(0, 0, m_Ht);
	const math::vec3 z2(0, m_er, m_Ht - Hr / 2);
	const math::vec3 z3(0, m_er / 2, m_Ht - Hr);
	m_zc = (M1 * z1 + M2 * z2 + M3 * z3) / (M1 + M2 + M3);
}
void Tensegrity::compute_energy(void)
{
	//data
	const double T = m_solver->m_T;
	const unsigned s = m_solver->m_step;
	const unsigned ns = m_solver->m_step_max;
	const double Ac = M_PI * m_dc * m_dc / 4;
	const math::vec3 u(m_solver->m_state_new + 0);
	const math::quat q(m_solver->m_state_new + 3);
	const math::vec3 v(m_solver->m_velocity_new + 0);
	const math::vec3 w(m_solver->m_velocity_new + 3);
	//kinetic
	double* data = m_solver->m_energy_data;
	data[3 * s + 0] = m_M * v.inner(v) / 2 + w.inner(m_J * w) / 2;
	//internal
	data[3 * s + 1] = 0;
	for(unsigned i = 0; i <= m_nc; i++)
	{
		const double L = cable_length(i);
		const double e = cable_strain_measure(i);
		if(e > 0) data[3 * s + 1] += m_Ec * e * e * Ac * L / 2;
	}
	//external
	data[3 * s + 2] = 0;
	const double t = s * T / ns;
	for(unsigned i = 0; i < m_pk.size(); i++)
	{
		const math::vec3 f = m_pk[i](t);
		const math::vec3 b = m_zc + u + q.rotate(m_ak[i] - m_zc);
		data[3 * s + 2] -= b.inner(f);
	}
}
void Tensegrity::compute_inertia(void)
{
	//data
	const double Hr = (m_Ht + m_Hc) / 2;
	const double Ar = m_type ? m_ar * m_br : M_PI * m_Rr * m_Rr;
	//mass
	const double M1 = m_pr * Ar * m_tr;
	const double M2 = m_pr * Hr * m_tl * m_tl;
	const double M3 = m_pr * m_er * m_tl * m_tl;
	//center
	const math::vec3 z1(0, 0, m_Ht);
	const math::vec3 z2(0, m_er, m_Ht - Hr / 2);
	const math::vec3 z3(0, m_er / 2, m_Ht - Hr);
	//inertia
	m_J.zeros();
	m_J(2, 2) += M2 * m_tl * m_tl / 6;
	m_J(1, 1) += M3 * m_tl * m_tl / 6;
	m_J(0, 0) += M2 * (Hr * Hr + m_tl * m_tl) / 12;
	m_J(1, 1) += M2 * (Hr * Hr + m_tl * m_tl) / 12;
	m_J(0, 0) += M3 * (m_tl * m_tl + m_er * m_er) / 12;
	m_J(2, 2) += M3 * (m_tl * m_tl + m_er * m_er) / 12;
	m_J -= M1 * (z1 - m_zc).spin() * (z1 - m_zc).spin();
	m_J -= M2 * (z2 - m_zc).spin() * (z2 - m_zc).spin();
	m_J -= M3 * (z3 - m_zc).spin() * (z3 - m_zc).spin();
	m_J(2, 2) += M1 * (m_type ? m_Rr * m_Rr / 2 : (m_ar * m_ar + m_br * m_br) / 12);
	m_J(0, 0) += M1 * (m_type ? m_tr * m_tr / 12 + m_Rr * m_Rr / 4 : (m_tr * m_tr + m_br * m_br) / 12);
	m_J(1, 1) += M1 * (m_type ? m_tr * m_tr / 12 + m_Rr * m_Rr / 4 : (m_tr * m_tr + m_ar * m_ar) / 12);
}
void Tensegrity::compute_acceleration(void)
{
	//data
	math::matrix M(6, 6);
	math::vector fi(6), fn(6), fe(6);
	math::vector a(m_solver->m_acceleration_new, 6);
	//compute
	inertia(M);
	inertial_force(fn);
	internal_force(fi);
	external_force(fe);
	M.solve(a, fe - fn - fi);
}

//formulation
void Tensegrity::inertia(math::matrix& M) const
{
	//data
	const math::quat q(m_solver->m_state_new + 3);
	//inertia
	M.zeros();
	M(0, 0) = M(1, 1) = M(2, 2) = m_M;
	M.span(3, 3) = q.rotation() * m_J;
}
void Tensegrity::damping(math::matrix& C) const
{
	//data
	const math::quat q(m_solver->m_state_new + 3);
	const math::vec3 w(m_solver->m_velocity_new + 3);
	//damping
	C.zeros();
	C.span(3, 3) = q.rotation() * (w.spin() * m_J - (m_J * w).spin());
}
void Tensegrity::stiffness(math::matrix& K) const
{
	//data
	K.zeros();
	const math::quat q(m_solver->m_state_new + 3);
	const math::vec3 w(m_solver->m_velocity_new + 3);
	const math::vec3 dw(m_solver->m_acceleration_new + 3);
	//internal force
	for(unsigned i = 0; i <= m_nc; i++)
	{
		//cable
		const double fk = cable_force(i);
		const double lk = cable_length(i);
		const double Kk = cable_stiffness(i);
		const math::vec3 x1 = position(i, 0, 1);
		const math::vec3 x2 = position(i, 1, 1);
		const math::vec3 z2 = position(i, 1, 0);
		const math::vec3 rk = q.rotate(z2 - m_zc);
		const math::vec3 tk = (x2 - x1).normalize();
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
	const double t = m_solver->m_step * m_solver->m_T / m_solver->m_step_max;
	for(unsigned i = 0; i < m_pk.size(); i++)
	{
		//force
		const math::vec3 ck = q.rotate(m_ak[i] - m_zc);
		const math::vec3 pk = m_solver ? m_pk[i](t) : m_solver->m_l_new * m_pk[i](0);
		//stiffness
		K.span(3, 3) -= pk.spin() * ck.spin();
	}
	//inertial force
	K.span(3, 3) -= q.rotate(m_J * dw + w.cross(m_J * w)).spin();
	//parametrization
	const math::quat q_old(m_solver->m_state_old + 3);
	const math::vec3 t_new(m_solver->m_dx.data() + 3);
	K.span(0, 3, 6, 3) = K.span(0, 3, 6, 3) * q_old.rotation() * t_new.rotation_gradient();
}
void Tensegrity::inertial_force(math::vector& fn) const
{
	//data
	fn.zeros();
	const math::quat q(m_solver->m_state_new + 3);
	const math::vec3 w(m_solver->m_velocity_new + 0);
	//inertial force
	math::vec3(fn.data() + 3) += q.rotate(w.cross(m_J * w));
}
void Tensegrity::internal_force(math::vector& fi) const
{
	//data
	fi.zeros();
	const double A = cable_area();
	const math::quat q(m_solver->m_state_new + 3);
	//internal force
	for(unsigned i = 0; i <= m_nc; i++)
	{
		//cable
		const double fk = cable_force(i);
		const math::vec3 x1 = position(i, 0, 1);
		const math::vec3 x2 = position(i, 1, 1);
		const math::vec3 z2 = position(i, 1, 0);
		const math::vec3 tk = (x2 - x1).normalize();
		//internal force
		math::vec3(fi.data() + 0) += fk * tk;
		math::vec3(fi.data() + 3) += q.rotate(z2 - m_zc).cross(fk * tk);
	}
}
void Tensegrity::external_force(math::vector& fe) const
{
	//data
	fe.zeros();
	const double T = m_solver->m_T;
	const unsigned s = m_solver->m_step;
	const unsigned ns = m_solver->m_step_max;
	const math::quat q(m_solver->m_state_new + 3);
	//external force
	const double t = s * T / ns;
	for(unsigned i = 0; i < m_pk.size(); i++)
	{
		math::vec3(fe.data() + 0) += m_pk[i](t);
		math::vec3(fe.data() + 3) += q.rotate(m_ak[i] - m_zc).cross(m_pk[i](t));
	}
}

//cables
double Tensegrity::cable_area(void) const
{
	return M_PI * m_dc * m_dc / 4;
}
double Tensegrity::cable_force(unsigned index) const
{
	const double A = cable_area();
	const double e = cable_strain_measure(index);
	const double g = cable_strain_gradient(index);
	return m_Ec * e * (e >= 0) * A * g;
}
double Tensegrity::cable_energy(unsigned index) const
{
	const double A = cable_area();
	const double L = cable_length(index);
	const double e = cable_strain_measure(index);
	return m_Ec * e * e * (e >= 0) * A * L / 2;
}
double Tensegrity::cable_length(unsigned index) const
{
	const math::vec3 z1 = position(index, 0, 0);
	const math::vec3 z2 = position(index, 1, 0);
	return (z2 - z1).norm();
}
double Tensegrity::cable_stretch(unsigned index) const
{
	const math::vec3 z1 = position(index, 0, 0);
	const math::vec3 z2 = position(index, 1, 0);
	const math::vec3 x1 = position(index, 0, 1);
	const math::vec3 x2 = position(index, 1, 1);
	return (x2 - x1).norm() / (z2 - z1).norm() + (index != 0) * m_a0;
}
double Tensegrity::cable_stiffness(unsigned index) const
{
	const double A = cable_area();
	const double L = cable_length(index);
	const double e = cable_strain_measure(index);
	const double h = cable_strain_hessian(index);
	const double g = cable_strain_gradient(index);
	return m_Ec * A * (g * g + e * h) / L * (e >= 0);
}
double Tensegrity::cable_strain_measure(unsigned index) const
{
	return log(cable_stretch(index));
}
double Tensegrity::cable_strain_hessian(unsigned index) const
{
	return -1 / pow(cable_stretch(index), 2);
}
double Tensegrity::cable_strain_gradient(unsigned index) const
{
	return 1 / cable_stretch(index);
}

//draw
void Tensegrity::draw_model_disks(canvas::Scene* scene) const
{
	//data
	const float tr = m_tr;
	const float Rr = m_Rr;
	const float Ht = m_Ht;
	canvas::objects::Cylinder* disk_1 = new canvas::objects::Cylinder;
	canvas::objects::Cylinder* disk_2 = new canvas::objects::Cylinder;
	//disk 1
	disk_1->radius(Rr);
	disk_1->height(tr);
	disk_1->color_fill({0, 0, 1});
	disk_1->color_stroke({0, 0, 0});
	//disk 2
	disk_2->radius(Rr);
	disk_2->height(tr);
	disk_2->shift({0, 0, Ht});
	disk_2->color_fill({0, 0, 1});
	disk_2->color_stroke({0, 0, 0});
	//scene
	scene->add_object(disk_1);
	scene->add_object(disk_2);
}
void Tensegrity::draw_model_links(canvas::Scene* scene) const
{
	//data
	const float tl = m_tl;
	const float er = m_er;
	const float tr = m_tr;
	const float Rr = m_Rr;
	const float Ht = m_Ht;
	const float Hc = m_Hc;
	const float Hr = (Ht + Hc - tl) / 2;
	canvas::objects::Cube* link_1 = new canvas::objects::Cube;
	canvas::objects::Cube* link_2 = new canvas::objects::Cube;
	canvas::objects::Cube* link_3 = new canvas::objects::Cube;
	canvas::objects::Cube* link_4 = new canvas::objects::Cube;
	//links
	link_1->color_fill({0, 0, 1});
	link_2->color_fill({0, 0, 1});
	link_3->color_fill({0, 0, 1});
	link_4->color_fill({0, 0, 1});
	link_1->color_stroke({0, 0, 0});
	link_2->color_stroke({0, 0, 0});
	link_3->color_stroke({0, 0, 0});
	link_4->color_stroke({0, 0, 0});
	//affine
	link_1->sizes({tl, tl, Hr});
	link_3->sizes({tl, tl, Hr});
	link_2->sizes({er - tl / 2, tl, tl});
	link_4->sizes({er - tl / 2, tl, tl});
	link_1->shift({er, 0, (Hr + tr) / 2});
	link_2->shift({er / 2 - tl / 4, 0, Hr});
	link_3->shift({-er, 0, Ht - (Hr + tr) / 2});
	link_4->shift({tl / 4 - er / 2, 0, tr + Hr - Hc});
	//scene
	scene->add_object(link_1);
	scene->add_object(link_2);
	scene->add_object(link_3);
	scene->add_object(link_4);
}
void Tensegrity::draw_model_latex(canvas::Scene* scene) const
{
	//data
	const float tl = m_tl;
	const float er = m_er;
	const float tr = m_tr;
	const float Rr = m_Rr;
	const float Ht = m_Ht;
	const float Hc = m_Hc;
	const float Hr = (Ht + Hc) / 2;
	canvas::objects::Latex* latex[5];
	const char* anchor[] = {"NC", "CW", "SC", "CE", "CW"};
	const char* source[] = {"$ R_r $", "$ H_t $", "$ e_r $", "$ H_r $", "$ H_c $"};
	//latex
	for(unsigned i = 0; i < 5; i++)
	{
		scene->add_latex(source[i]);
		latex[i] = new canvas::objects::Latex;
		latex[i]->index(i);
		latex[i]->size(2 * tl);
		latex[i]->anchor(anchor[i]);
		latex[i]->color_fill({1, 0, 0});
		latex[i]->rotate({M_PI_2, 0, 0});
		scene->add_object(latex[i]);
	}
	latex[1]->shift({Rr + tl, 0, Ht / 2});
	latex[2]->shift({er / 2, 0, tr + Hr});
	latex[0]->shift({Rr / 2, 0, -tl - tr / 2});
	latex[4]->shift({er + tl + tl / 2, 0, Hr - Hc / 2});
	latex[3]->shift({-Rr - tl, 0, Ht - Hr / 2 + tl / 4});
}
void Tensegrity::draw_model_cables(canvas::Scene* scene) const
{
	//data
	const float tl = m_tl;
	const float tr = m_tr;
	const float Rr = m_Rr;
	const float Ht = m_Ht;
	const float Hc = m_Hc;
	canvas::objects::Line* cable;
	const float Hr = (Ht + Hc) / 2;
	//inner
	cable = new canvas::objects::Line;
	cable->color_stroke("dark green");
	cable->point(0, {0, 0, Hr - tl});
	cable->point(1, {0, 0, Hr + tr - Hc});
	scene->add_object(cable);
	//outer
	for(unsigned i = 0; i < m_nc; i++)
	{
		cable = new canvas::objects::Line;
		cable->color_stroke("dark green");
		cable->point(0, {Rr * cosf(2 * M_PI * i / m_nc), Rr * sinf(2 * M_PI * i / m_nc), tr / 2});
		cable->point(1, {Rr * cosf(2 * M_PI * i / m_nc), Rr * sinf(2 * M_PI * i / m_nc), Ht - tr / 2});
		scene->add_object(cable);
	}
}
void Tensegrity::draw_model_guides(canvas::Scene* scene) const
{
	//data
	const float tl = m_tl;
	const float er = m_er;
	const float tr = m_tr;
	const float Rr = m_Rr;
	const float Ht = m_Ht;
	const float Hc = m_Hc;
	const float Hr = (Ht + Hc) / 2;
	canvas::objects::Line* lines[5];
	//lines
	for(unsigned i = 0; i < sizeof(lines) / sizeof(canvas::objects::Line*); i++)
	{
		lines[i] = new canvas::objects::Line;
		lines[i]->add_arrow(0, false);
		lines[i]->add_arrow(1, false);
		lines[i]->arrow(0)->width(0);
		lines[i]->arrow(1)->width(0);
		lines[i]->arrow(0)->height(tl);
		lines[i]->arrow(1)->height(tl);
		lines[i]->color_stroke({1, 0, 0});
		lines[i]->arrow(0)->color_stroke({1, 0, 0});
		lines[i]->arrow(1)->color_stroke({1, 0, 0});
		scene->add_object(lines[i]);
	}
	//guide Rr
	lines[0]->point(0, {0, 0, -tl - tr / 2});
	lines[0]->point(1, {Rr, 0, -tl - tr / 2});
	//guide Ht
	lines[1]->point(0, {Rr + tl, 0, 0});
	lines[1]->point(1, {Rr + tl, 0, Ht});
	//guide er
	lines[2]->point(0, {0, 0, tr + Hr});
	lines[2]->point(1, {er, 0, tr + Hr});
	//guide Hr
	lines[3]->point(0, {-Rr - tl, 0, Ht});
	lines[3]->point(1, {-Rr - tl, 0, Ht - Hr + tl / 2});
	//guide Hc
	lines[4]->point(0, {er + tl + tl / 2, 0, Hr - tl / 2});
	lines[4]->point(1, {er + tl + tl / 2, 0, Hr + tl / 2 - Hc});
}

//position
math::vec3 Tensegrity::position(unsigned index, bool level, bool configuration) const
{
	if(!level || !configuration)
	{
		math::vec3 x;
		const double Hr = (m_Ht + m_Hc) / 2;
		x[0] = (index != 0) * m_Rr * cos(2 * M_PI * index / m_nc);
		x[1] = (index != 0) * m_Rr * sin(2 * M_PI * index / m_nc);
		x[2] = (index == 0) * (Hr - level * m_Hc) + (index != 0) * level * m_Ht;
		return x;
	}
	else
	{
		const math::vec3 uc(m_solver->m_state_new + 0);
		const math::quat qc(m_solver->m_state_new + 3);
		return m_zc + uc + qc.rotate(position(index, 1, 0) - m_zc);
	}
}