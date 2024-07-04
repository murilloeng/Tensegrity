//tensegrity
#include "Tensegrity/inc/Solver.hpp"
#include "Tensegrity/inc/Window.hpp"

//uic
#include "Tensegrity/build/uic/Window.hpp"

//constructor
Window::Window(void) : m_ui(new Ui::Window)
{
	m_ui->setupUi(this);
	m_tensegrity.m_nc = 3;
	m_tensegrity.m_Ht = 3.20e-01;
	m_tensegrity.m_Hc = 1.40e-01;
	m_tensegrity.m_Rr = 1.40e-01;
	m_tensegrity.m_tr = 1.00e-02;
	m_tensegrity.m_tl = 1.00e-02;
	m_tensegrity.m_dc = 1.50e-03;
	m_tensegrity.m_Ec = 2.00e+11;
	m_ui->canvas->m_tensegrity = &m_tensegrity;
	m_ui->edit_state_u1->setText(QString::asprintf("%+.2e", m_tensegrity.m_solver->m_dx[0]));
	m_ui->edit_state_u2->setText(QString::asprintf("%+.2e", m_tensegrity.m_solver->m_dx[1]));
	m_ui->edit_state_u3->setText(QString::asprintf("%+.2e", m_tensegrity.m_solver->m_dx[2]));
	m_ui->edit_state_t1->setText(QString::asprintf("%+.2e", m_tensegrity.m_solver->m_dx[3]));
	m_ui->edit_state_t2->setText(QString::asprintf("%+.2e", m_tensegrity.m_solver->m_dx[4]));
	m_ui->edit_state_t3->setText(QString::asprintf("%+.2e", m_tensegrity.m_solver->m_dx[5]));
}

//destructor
Window::~Window(void)
{
	delete m_ui;
}