//qt
#include <QtGui/QKeyEvent>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QGridLayout>

//canvas
#include "Canvas/inc/Scene/Scene.hpp"

//tensegrity
#include "Tensegrity/inc/Canvas.hpp"
#include "Tensegrity/inc/Solver.hpp"
#include "Tensegrity/inc/Deformed.hpp"

//constructors
Deformed::Deformed(QWidget* parent, const Tensegrity* tensegrity) : 
	QWidget(parent), m_tensegrity(tensegrity)
{
	//data
	m_label = new QLabel(this);
	m_canvas = new Canvas(this);
	QGridLayout* grid_layout = new QGridLayout(this);
	const uint32_t ns = m_tensegrity->solver()->step_max();
	m_slider = new QSlider(Qt::Orientation::Horizontal, this);
	//widgets
	m_slider->setMinimum(0);
	m_slider->setMaximum(ns);
	m_label->setMaximumHeight(30);
	grid_layout->addWidget(m_label, 0, 0);
	grid_layout->addWidget(m_slider, 1, 0);
	grid_layout->addWidget(m_canvas, 2, 0);
	//connect
	QObject::connect(m_slider, &QSlider::valueChanged, this, &Deformed::update);
}

//destructor
Deformed::~Deformed(void)
{
	return;
}

//update
void Deformed::update(void)
{
	update_label();
	update_canvas();
}
void Deformed::update_label(void)
{
	//data
	const uint32_t step = m_slider->value();
	const uint16_t step_max = m_tensegrity->solver()->step_max();
	const double* l = m_tensegrity->solver()->m_solver_data + step;
	const double* u = m_tensegrity->solver()->m_state_data + 7 * step;
	//text
	const math::vec3 r = math::quat(u + 3).pseudo();
	const QString sl = QString::asprintf("load: %+.2e ", l[0]);
	const QString ss = QString::asprintf("step: %d/%d ", step, step_max);
	const QString su = QString::asprintf("u1: %+.2e u2: %+.2e u3: %+.2e ", u[0], u[1], u[2]);
	const QString sr = QString::asprintf("r1: %+.2e r2: %+.2e r3: %+.2e ", r[0], r[1], r[2]);
	//widget
	m_label->setText(ss + su + sr + sl);
}
void Deformed::update_canvas(void)
{
	const uint32_t step = m_slider->value();
	m_tensegrity->draw_deformed(m_canvas->scene(), step);
	m_canvas->scene()->update(true);
	m_canvas->update();
}

//slots
void Deformed::slot_slider(void)
{
	return;
}

//events
void Deformed::keyPressEvent(QKeyEvent* event)
{
	m_canvas->keyPressEvent(event);
}