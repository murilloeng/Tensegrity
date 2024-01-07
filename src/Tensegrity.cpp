//std
#include <cmath>

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
	m_er(1.00e-01), m_tl(1.00e-02), m_tr(1.00e-02), 
	m_Rr(2.00e-01), m_Ht(5.00e-01), m_Hc(2.50e-01), m_Ec(8.00e+10), m_dc(1.00e-03), 
	m_nc(3), m_state_data(nullptr), m_velocity_data(nullptr), m_acceleration_data(nullptr)
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

//cables
double Tensegrity::cable_stretch(unsigned index) const
{
	const math::vec3 z1 = position(index, 0, 0);
	const math::vec3 z2 = position(index, 1, 0);
	const math::vec3 x1 = position(index, 0, 1);
	const math::vec3 x2 = position(index, 1, 1);
	return (x2 - x1).norm() / (z2 - z1).norm();
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
		// return m_x + m_q.rotate(position(index, 1, 0) - math::vec3(0, 0, m_Ht));
	}
}

//formulation
math::vector Tensegrity::residue(void) const
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
		// math::vec3(fi.data() + 3) += s * Ac * de * (x2 - m_x).cross(tc);
	}
	//return
	return fi;
}
math::matrix Tensegrity::stiffness(void) const
{
	//data
	math::matrix K(6, 6, math::mode::zeros);
	//stiffness

	//return
	return K;
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