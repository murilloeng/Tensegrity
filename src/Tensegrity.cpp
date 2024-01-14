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
#include "inc/Tensegrity.hpp"

//constructors
Tensegrity::Tensegrity(void) : 
	m_nc(3), m_type(0), m_steps(1000), m_solver(0), m_iteration_max(10), 
	m_T(1.00e+00), m_pr(4.00e+02), m_er(1.00e-01), m_tl(1.00e-02), m_tr(1.00e-02), m_ar(2.00e-01), 
	m_br(2.00e-01), m_Rr(2.00e-01), m_Ht(5.00e-01), m_Hc(2.50e-01), m_Ec(8.00e+10), m_dc(1.00e-03), 
	m_state_data(nullptr), m_energy_data(nullptr), m_velocity_data(nullptr), m_acceleration_data(nullptr),
	m_r(6), m_fn(6), m_fi(6), m_fe(6), m_Kt(6, 6), m_Ct(6, 6), m_Mt(6, 6), m_dx(6), m_dxt(6), m_ddxt(6), m_ddxr(6)
{
	memset(m_state_old, 0, 7 * sizeof(double));
	memset(m_state_new, 0, 7 * sizeof(double));
	m_state_old[3] = m_state_new[3] = 1.00e+00;
	memset(m_velocity_old, 0, 6 * sizeof(double));
	memset(m_velocity_new, 0, 6 * sizeof(double));
	memset(m_acceleration_old, 0, 6 * sizeof(double));
	memset(m_acceleration_new, 0, 6 * sizeof(double));
}

//destructor
Tensegrity::~Tensegrity(void)
{
	delete[] m_state_data;
	delete[] m_energy_data;
	delete[] m_velocity_data;
	delete[] m_acceleration_data;
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

//solver
void Tensegrity::solve(void)
{
	setup();
	!m_solver ? solve_static() : solve_dynamic();
}

//solver
void Tensegrity::setup(void)
{
	//inertia
	compute_mass();
	compute_center();
	compute_inertia();
	//setup
	m_l_new = m_l_old = 0;
	//allocate
	delete[] m_state_data;
	delete[] m_energy_data;
	delete[] m_velocity_data;
	delete[] m_acceleration_data;
	m_state_data = new double[7 * (m_steps + 1)];
	m_energy_data = new double[3 * (m_steps + 1)];
	m_velocity_data = new double[6 * (m_steps + 1)];
	m_acceleration_data = new double[6 * (m_steps + 1)];
	memcpy(m_state_new, m_state_old, 7 * sizeof(double));
	memcpy(m_state_data, m_state_old, 7 * sizeof(double));
	memcpy(m_velocity_new, m_velocity_old, 6 * sizeof(double));
	memcpy(m_velocity_data, m_velocity_old, 6 * sizeof(double));
	memcpy(m_acceleration_new, m_acceleration_old, 6 * sizeof(double));
	memcpy(m_acceleration_data, m_acceleration_old, 6 * sizeof(double));
}
void Tensegrity::record(void)
{
	compute_energy();
	memcpy(m_state_data + 7 * m_step, m_state_new, 7 * sizeof(double));
	memcpy(m_velocity_data + 6 * m_step, m_velocity_new, 6 * sizeof(double));
	memcpy(m_acceleration_data + 6 * m_step, m_acceleration_new, 6 * sizeof(double));
}
void Tensegrity::update(void)
{
	m_l_old = m_l_new;
	memcpy(m_state_old, m_state_new, 7 * sizeof(double));
	memcpy(m_velocity_old, m_velocity_new, 6 * sizeof(double));
	memcpy(m_acceleration_old, m_acceleration_new, 6 * sizeof(double));
}
void Tensegrity::restore(void)
{
	m_l_new = m_l_old;
	memcpy(m_state_new, m_state_old, 7 * sizeof(double));
	memcpy(m_velocity_new, m_velocity_old, 6 * sizeof(double));
	memcpy(m_acceleration_new, m_acceleration_old, 6 * sizeof(double));
}
void Tensegrity::update_state(void)
{
	//data
	math::vec3 u_new(m_state_new + 0);
	math::quat q_new(m_state_new + 3);
	const math::vec3 u_old(m_state_old + 0);
	const math::quat q_old(m_state_old + 3);
	const math::vec3 du_new(m_dx.data() + 0);
	const math::vec3 dt_new(m_dx.data() + 3);
	//update
	u_new = u_old + du_new;
	q_new = q_old * dt_new.quaternion();
}
void Tensegrity::solve_static(void)
{
	//loop
	stiffness(m_Kt);
	external_force(m_fe);
	for(m_step = 0; m_step < m_steps; m_step++)
	{
		//predictor
		m_Kt.solve(m_dxt, m_fe);
		compute_load_predictor();
		m_dx = m_dl * m_dxt;
		//update
		update_state();
		m_l_new = m_l_old + m_dl;
		//corrector
		for(m_iteration = 0; m_iteration < m_iteration_max; m_iteration++)
		{
			stiffness(m_Kt);
			internal_force(m_fi);
			m_r = m_l_new * m_fe - m_fi;
			if(m_r.norm() < 1e-5 * m_fe.norm())
			{
				printf("step: %04d iterations: %04d load: %+.2e\n", m_step, m_iteration, m_l_new);
				record();
				update();
				break;
			}
			m_Kt.solve(m_ddxr, m_r);
			m_Kt.solve(m_ddxt, m_fe);
			compute_load_corrector();
			m_dl += m_ddl;
			m_dx += m_ddxr + m_ddl * m_ddxt;
			update_state();
			m_l_new = m_l_old + m_dl;
		}
	}
}
void Tensegrity::solve_dynamic(void)
{

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
	const math::vec3 u(m_state_new + 0);
	const math::quat q(m_state_new + 3);
	const math::vec3 v(m_velocity_new + 0);
	const math::vec3 w(m_velocity_new + 3);
	const double t = m_step * m_T / m_steps;
	const double Ac = M_PI * m_dc * m_dc / 4;
	//kinetic
	m_energy_data[3 * m_step + 0] = m_M * v.inner(v) / 2 + w.inner(m_J * w) / 2;
	//internal
	m_energy_data[3 * m_step + 1] = 0;
	for(unsigned i = 0; i <= m_nc; i++)
	{
		const double L = cable_length(i);
		const double e = cable_strain_measure(i);
		if(e > 0) m_energy_data[3 * m_step + 1] += m_Ec * e * e * Ac * L / 2;
	}
	//external
	m_energy_data[3 * m_step + 2] = 0;
	for(unsigned i = 0; i < m_pk.size(); i++)
	{
		const math::vec3 f = m_pk[i](t);
		const math::vec3 b = m_zc + u + q.rotate(m_ak[i] - m_zc);
		m_energy_data[3 * m_step + 2] -= b.inner(f);
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
	m_J(0, 0) += M2 * (Hr * Hr + m_tl * m_tl) / 12;
	m_J(1, 1) += M2 * (Hr * Hr + m_tl * m_tl) / 12;
	m_J(1, 1) += M3 * m_tl * m_tl / 6;
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
	math::vector a(m_acceleration_new, 6);
	//compute
	inertia(M);
	inertial_force(fn);
	internal_force(fi);
	external_force(fe);
	M.solve(a, fe - fn - fi);
}
void Tensegrity::compute_load_predictor(void)
{
	if(m_step != 0)
	{
		m_dl = math::sign(m_dxt.inner(m_dx)) * m_dx.norm() / m_dxt.norm();
	}
}
void Tensegrity::compute_load_corrector(void)
{
	const double s = m_ddxt.inner(m_dx);
	const double a = m_ddxt.inner(m_ddxt);
	const double b = m_ddxt.inner(m_ddxr + m_dx);
	const double c = m_ddxr.inner(m_ddxr + 2 * m_dx);
	m_ddl = -b / a + math::sign(s) * sqrt(pow(b / a, 2) - c / a);
}

//formulation
void Tensegrity::inertia(math::matrix& M) const
{
	//data
	const math::quat q(m_state_new + 3);
	//inertia
	M.zeros();
	M(0, 0) = M(1, 1) = M(2, 2) = m_M;
	M.span(3, 3) = q.rotation() * m_J;
}
void Tensegrity::damping(math::matrix& C) const
{
	//data
	const math::quat q(m_state_new + 3);
	const math::vec3 w(m_velocity_new + 3);
	//damping
	C.zeros();
	C.span(3, 3) = q.rotation() * (w.spin() * m_J - (m_J * w).spin());
}
void Tensegrity::stiffness(math::matrix& K) const
{
	//data
	K.zeros();
	const math::quat q(m_state_new + 3);
	const math::vec3 w(m_velocity_new + 3);
	const math::vec3 dw(m_acceleration_new + 3);
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
	const double t = m_step * m_T / m_steps;
	for(unsigned i = 0; i < m_pk.size(); i++)
	{
		//force
		const math::vec3 ck = q.rotate(m_ak[i] - m_zc);
		const math::vec3 pk = m_solver ? m_pk[i](t) : m_l_new * m_pk[i](0);
		//stiffness
		K.span(3, 3) -= pk.spin() * ck.spin();
	}
	//inertial force
	K.span(3, 3) -= q.rotate(m_J * dw + w.cross(m_J * w)).spin();
	//parametrization
	const math::quat q_old(m_state_old + 3);
	const math::vec3 t_new(m_dx.data() + 3);
	K.span(0, 3, 6, 3) = K.span(0, 3, 6, 3) * q_old.rotation() * t_new.rotation_gradient();
}
void Tensegrity::inertial_force(math::vector& fn) const
{
	//data
	fn.zeros();
	const math::quat q(m_state_new + 3);
	const math::vec3 w(m_velocity_new + 0);
	//inertial force
	math::vec3(fn.data() + 3) += q.rotate(w.cross(m_J * w));
}
void Tensegrity::internal_force(math::vector& fi) const
{
	//data
	fi.zeros();
	const double A = cable_area();
	const math::quat q(m_state_new + 3);
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
	const math::quat q(m_state_new + 3);
	//external force
	const double t = m_step * m_T / m_steps;
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
	return (x2 - x1).norm() / (z2 - z1).norm();
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
		const math::vec3 uc(m_state_new + 0);
		const math::quat qc(m_state_new + 3);
		return m_zc + uc + qc.rotate(position(index, 1, 0) - m_zc);
	}
}