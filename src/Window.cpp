//qt
#include <QtWidgets/QTabBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QGridLayout>

//tensegrity
#include "Tensegrity/inc/Path.hpp"
#include "Tensegrity/inc/Model.hpp"
#include "Tensegrity/inc/Solver.hpp"
#include "Tensegrity/inc/Window.hpp"
#include "Tensegrity/inc/Deformed.hpp"

//constructors
Window::Window(const Tensegrity* tensegrity) : m_tensegrity(tensegrity)
{
	//data
	QWidget* widget = new QWidget(this);
	QTabWidget* tabs = new QTabWidget(this);
	QGridLayout* grid_layout = new QGridLayout(widget);
	//widgets
	m_path = new Path(this, tensegrity);
	m_model = new Model(this, tensegrity);
	m_deformed = new Deformed(this, tensegrity);
	//tabs
	tabs->addTab(m_model, "Model");
	tabs->addTab(m_deformed, "Deformed");
	tabs->addTab(m_path, "Equilibrium Path");
	tabs->tabBar()->setTabEnabled(1, tensegrity->solver()->equilibrium());
	tabs->tabBar()->setTabEnabled(2, tensegrity->solver()->equilibrium());
	//grid
	grid_layout->addWidget(tabs);
	//central
	setCentralWidget(widget);
	//connect
	QObject::connect(tabs, &QTabWidget::currentChanged, this, &Window::slot_tabs);
}

//destructor
Window::~Window(void)
{
	return;
}

//data
Path* Window::path(void)
{
	return m_path;
}
Model* Window::model(void)
{
	return m_model;
}
Deformed* Window::deformed(void)
{
	return m_deformed;
}

//slots
void Window::slot_tabs(int32_t index)
{
	if(index == 1)
	{
		m_deformed->update_label();
		m_deformed->update_canvas();
	}
}