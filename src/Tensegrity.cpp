//std
#include <cmath>

//math
#include "Math/inc/misc/misc.hpp"

//canvas
#include "Canvas/inc/Objects/1D/Line.hpp"
#include "Canvas/inc/Objects/1D/Arrow.hpp"

#include "Canvas/inc/Objects/3D/Cube.hpp"
#include "Canvas/inc/Objects/3D/Cylinder.hpp"

#include "Canvas/inc/Objects/Image/Latex.hpp"

//Tensegrity
#include "Tensegrity/inc/Solver.hpp"
#include "Tensegrity/inc/Tensegrity.hpp"

//constructors
Tensegrity::Tensegrity(void) : 
	m_pr(4.00e+02), m_er(5.00e-02), m_tl(1.00e-02), m_tr(1.00e-02), m_ar(2.00e-01), m_br(2.00e-01), m_Rr(1.40e-01), m_Ht(3.20e-01), m_Hc(1.40e-01), m_Ec(2.00e+11), m_dc(1.50e-03), m_q0(0.00e+00), m_sr(0.00e+00), m_sy(4.00e+08), m_nc(3), m_type(0), m_inelastic(false), m_solver(new Solver(this))
{
	sprintf(m_label, "Tensegrity");
	memset(m_K0, 0, 6 * sizeof(double));
}

//destructor
Tensegrity::~Tensegrity(void)
{
	delete m_solver;
}

//draw
void Tensegrity::draw_model(canvas::Scene* scene) const
{
	draw_disks(scene);
	draw_links(scene);
	draw_latex(scene);
	draw_cables(scene);
	draw_guides(scene);
	scene->background({1, 1, 1, 1});
	scene->camera().rotation({float(M_PI), 0, 0});
}
void Tensegrity::draw_configuration(canvas::Scene* scene) const
{
	draw_disks(scene, true);
	draw_links(scene, true);
	draw_cables(scene, true);
	scene->background({1, 1, 1, 1});
	scene->camera().rotation({float(M_PI), 0, 0});
}

//compute
void Tensegrity::compute_energy(void)
{
	//data
	const double T = m_solver->m_T;
	const unsigned s = m_solver->m_step;
	const unsigned ns = m_solver->m_step_max;
	const math::vec3 zr(0, 0, m_Ht);
	const math::vec3 ur(m_solver->m_state_new + 0);
	const math::quat qr(m_solver->m_state_new + 3);
	const math::vec3 vr(m_solver->m_velocity_new + 0);
	const math::vec3 wr(m_solver->m_velocity_new + 3);
	//kinetic
	double* data = m_solver->m_energy_data;
	data[3 * s + 0] = (m_M * vr.inner(vr) + 2 * vr.inner(m_J1 * wr) + wr.inner(m_J2 * wr)) / 2;
	//internal
	data[3 * s + 1] = internal_energy();
	//external
	data[3 * s + 2] = 0;
	const double t = s * T / ns;
	for(unsigned i = 0; i < m_pk.size(); i++)
	{
		const math::vec3 f = m_pk[i];
		const math::vec3 b = zr + ur + qr.rotate(m_ak[i] - zr);
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
	m_M = M1 + M2 + M3;
	//center
	const math::vec3 z1(0, 0, m_Ht);
	const math::vec3 z2(0, m_er, m_Ht - Hr / 2);
	const math::vec3 z3(0, m_er / 2, m_Ht - Hr);
	m_zc = (M1 * z1 + M2 * z2 + M3 * z3) / (M1 + M2 + M3);
	//inertia
	m_J2.zeros();
	m_J2(2, 2) += M2 * m_tl * m_tl / 6;
	m_J2(1, 1) += M3 * m_tl * m_tl / 6;
	m_J2(0, 0) += M2 * (Hr * Hr + m_tl * m_tl) / 12;
	m_J2(1, 1) += M2 * (Hr * Hr + m_tl * m_tl) / 12;
	m_J2(0, 0) += M3 * (m_tl * m_tl + m_er * m_er) / 12;
	m_J2(2, 2) += M3 * (m_tl * m_tl + m_er * m_er) / 12;
	m_J2 -= M1 * (z1 - m_zc).spin() * (z1 - m_zc).spin();
	m_J2 -= M2 * (z2 - m_zc).spin() * (z2 - m_zc).spin();
	m_J2 -= M3 * (z3 - m_zc).spin() * (z3 - m_zc).spin();
	m_J2(2, 2) += M1 * (m_type ? m_Rr * m_Rr / 2 : (m_ar * m_ar + m_br * m_br) / 12);
	m_J2(0, 0) += M1 * (m_type ? m_tr * m_tr / 12 + m_Rr * m_Rr / 4 : (m_tr * m_tr + m_br * m_br) / 12);
	m_J2(1, 1) += M1 * (m_type ? m_tr * m_tr / 12 + m_Rr * m_Rr / 4 : (m_tr * m_tr + m_ar * m_ar) / 12);
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
	M.span(3, 3) = +m_J2;
	M.span(0, 3) = +m_J1;
	M.span(3, 0) = -m_J1;
	M.span(0, 0) = m_M * math::mat3::eye();
}
void Tensegrity::damping(math::matrix& C) const
{
	//data
	const math::vec3 wr(m_solver->m_velocity_new + 3);
	//damping
	C.zeros();
	C.span(3, 3) = wr.spin() * m_J2 - (m_J2 * wr).spin();
}
void Tensegrity::stiffness(math::matrix& K) const
{
	//data
	K.zeros();
	const math::vec3 zr(0, 0, m_Ht);
	const math::quat qr(m_solver->m_state_new + 3);
	const double A = M_PI * m_dc * m_dc / 4;
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
		const double sk = fmax(m_Ec * ek + m_sr, 0);
		const double Ck = m_Ec * (m_Ec * ek + m_sr >= 0);
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

double Tensegrity::kinetic_energy(void) const
{
	//data
	const math::vec3 vr(m_solver->m_velocity_new + 0);
	const math::vec3 wr(m_solver->m_velocity_new + 3);
	//return
	return (m_M * vr.inner(vr) + 2 * vr.inner(m_J1 * wr) + wr.inner(m_J2 * wr)) / 2;
}
double Tensegrity::internal_energy(void) const
{
	//data
	double U = 0;
	const double Ac = M_PI * m_dc * m_dc / 4;
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
		const double ek = log(ak) + m_sr / m_Ec;
		//energy
		U += m_Ec * Ac * Lk / 2 * ek * ek * (ek > 0);
	}
	//return
	return U;
}
double Tensegrity::potential_energy(void) const
{
	//data
	double V = 0;
	const double T = m_solver->m_T;
	const unsigned s = m_solver->m_step;
	const unsigned ns = m_solver->m_step_max;
	const double t = m_solver->m_type * s * T / ns;
	const math::vec3 qr(m_solver->m_state_new + 3);
	//potential energy
	const math::vec3 zr(0, 0, m_Ht);
	for(unsigned i = 0; i < m_pk.size(); i++)
	{
		V -= qr.rotate(m_ak[i] - zr).inner(m_pk[i]);
	}
	//return
	return V;
}

void Tensegrity::inertial_force(math::vector& fn) const
{
	//data
	fn.zeros();
	const math::quat q(m_solver->m_state_new + 3);
	const math::vec3 w(m_solver->m_velocity_new + 0);
	//inertial force
	math::vec3(fn.data() + 3) += q.rotate(w.cross(m_J2 * w));
}
void Tensegrity::internal_force(math::vector& fi) const
{
	//data
	fi.zeros();
	const math::vec3 zr(0, 0, m_Ht);
	const math::quat qr(m_solver->m_state_new + 3);
	const double A = M_PI * m_dc * m_dc / 4;
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
		const double se = m_Ec * ek + m_sr;
		const double sc = se > 0 ? se : 0;
		const double sk = m_inelastic && (sc > m_sy) ? m_sy : sc;
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
	const math::quat qr(m_solver->m_state_new + 3);
	//external force
	const math::vec3 zr(0, 0, m_Ht);
	for(unsigned i = 0; i < m_pk.size(); i++)
	{
		math::vec3(fe.data() + 0) += m_pk[i];
		math::vec3(fe.data() + 3) += qr.rotate(m_ak[i] - zr).cross(m_pk[i]);
	}
	for(unsigned i = 0; i < m_mk.size(); i++)
	{
		math::vec3(fe.data() + 3) += m_mk[i];
	}
}

//draw
void Tensegrity::draw_disks(canvas::Scene* scene, bool state) const
{
	//data
	const float tr = float(m_tr);
	const float Rr = float(m_Rr);
	const float Ht = float(m_Ht);
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
void Tensegrity::draw_links(canvas::Scene* scene, bool state) const
{
	//data
	const float tl = float(m_tl);
	const float er = float(m_er);
	const float tr = float(m_tr);
	const float Rr = float(m_Rr);
	const float Ht = float(m_Ht);
	const float Hc = float(m_Hc);
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
void Tensegrity::draw_latex(canvas::Scene* scene, bool state) const
{
	//data
	const float tl = float(m_tl);
	const float er = float(m_er);
	const float tr = float(m_tr);
	const float Rr = float(m_Rr);
	const float Ht = float(m_Ht);
	const float Hc = float(m_Hc);
	const float Hr = (Ht + Hc) / 2;
	canvas::objects::Latex* latex[5];
	const char* anchor[] = {"SC", "CW", "NC", "CE", "CW"};
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
		latex[i]->direction(0, {1, 0, 0});
		latex[i]->direction(1, {0, 0, 1});
		latex[i]->rotate({float(M_PI), 0, 0});
		scene->add_object(latex[i]);
	}
	latex[1]->shift({Rr + tl, 0, Ht / 2});
	latex[2]->shift({er / 2, 0, tr + Hr});
	latex[0]->shift({Rr / 2, 0, -tl - tr / 2});
	latex[4]->shift({er + tl + tl / 2, 0, Hr - Hc / 2});
	latex[3]->shift({-Rr - tl, 0, Ht - Hr / 2 + tl / 4});
}
void Tensegrity::draw_cables(canvas::Scene* scene, bool state) const
{
	//data
	const float tl = float(m_tl);
	const float tr = float(m_tr);
	const float Rr = float(m_Rr);
	const float Ht = float(m_Ht);
	const float Hc = float(m_Hc);
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
		cable->point(0, {Rr * cosf(2 * float(M_PI) * i / m_nc), Rr * sinf(2 * float(M_PI) * i / m_nc), tr / 2 });
		cable->point(1, {Rr * cosf(2 * float(M_PI) * i / m_nc), Rr * sinf(2 * float(M_PI) * i / m_nc), Ht - tr / 2});
		scene->add_object(cable);
	}
}
void Tensegrity::draw_guides(canvas::Scene* scene, bool state) const
{
	//data
	const float tl = float(m_tl);
	const float er = float(m_er);
	const float tr = float(m_tr);
	const float Rr = float(m_Rr);
	const float Ht = float(m_Ht);
	const float Hc = float(m_Hc);
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
		const double Hr = (m_Ht + m_Hc) / 2;
		return {
			(index != 0) * m_Rr * cos(2 * M_PI * index / m_nc + m_q0 * level),
			(index != 0) * m_Rr * sin(2 * M_PI * index / m_nc + m_q0 * level),
			(index == 0) * (Hr - level * m_Hc) + (index != 0) * level * m_Ht
		};
	}
	else
	{
		const math::vec3 zr(0, 0, m_Ht);
		const math::vec3 ur(m_solver->m_state_new + 0);
		const math::quat qr(m_solver->m_state_new + 3);
		return zr + ur + qr.rotate(position(index, level, 0) - zr);
	}
}