//tensegrity
#include "Tensegrity/inc/Solver.hpp"
#include "Tensegrity/inc/Window.hpp"

//uic
#include "Tensegrity/build/uic/Window.hpp"

//constructor
Window::Window(void) : m_ui(new Ui::Window)
{
	setup();
	connect();
}

//destructor
Window::~Window(void)
{
	delete m_ui;
}

//setup
void Window::setup(void)
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
	m_tensegrity.m_solver->m_dx.zeros();
	m_ui->canvas->m_tensegrity = &m_tensegrity;
	m_ui->edit_model_Ht->setText(QString::asprintf("%+.2e", m_tensegrity.m_Ht));
	m_ui->edit_model_Hc->setText(QString::asprintf("%+.2e", m_tensegrity.m_Hc));
	m_ui->edit_model_Rr->setText(QString::asprintf("%+.2e", m_tensegrity.m_Rr));
	m_ui->edit_model_er->setText(QString::asprintf("%+.2e", m_tensegrity.m_er));
	m_ui->edit_model_tr->setText(QString::asprintf("%+.2e", m_tensegrity.m_tr));
	m_ui->edit_model_tl->setText(QString::asprintf("%+.2e", m_tensegrity.m_tl));
	m_ui->edit_model_dc->setText(QString::asprintf("%+.2e", m_tensegrity.m_dc));
	m_ui->edit_model_Ec->setText(QString::asprintf("%+.2e", m_tensegrity.m_Ec));
	m_ui->edit_model_sr->setText(QString::asprintf("%+.2e", m_tensegrity.m_sr));
	m_ui->edit_state_u1->setText(QString::asprintf("%+.2e", m_tensegrity.m_solver->m_dx[0]));
	m_ui->edit_state_u2->setText(QString::asprintf("%+.2e", m_tensegrity.m_solver->m_dx[1]));
	m_ui->edit_state_u3->setText(QString::asprintf("%+.2e", m_tensegrity.m_solver->m_dx[2]));
	m_ui->edit_state_t1->setText(QString::asprintf("%+.2e", m_tensegrity.m_solver->m_dx[3]));
	m_ui->edit_state_t2->setText(QString::asprintf("%+.2e", m_tensegrity.m_solver->m_dx[4]));
	m_ui->edit_state_t3->setText(QString::asprintf("%+.2e", m_tensegrity.m_solver->m_dx[5]));
}
void Window::connect(void)
{
	QObject::connect(m_ui->edit_model_Hc, &QLineEdit::editingFinished, this, &Window::slot_edit_model);
	QObject::connect(m_ui->edit_model_Ht, &QLineEdit::editingFinished, this, &Window::slot_edit_model);
	QObject::connect(m_ui->edit_model_Rr, &QLineEdit::editingFinished, this, &Window::slot_edit_model);
	QObject::connect(m_ui->edit_model_er, &QLineEdit::editingFinished, this, &Window::slot_edit_model);
	QObject::connect(m_ui->edit_model_tr, &QLineEdit::editingFinished, this, &Window::slot_edit_model);
	QObject::connect(m_ui->edit_model_tl, &QLineEdit::editingFinished, this, &Window::slot_edit_model);
	QObject::connect(m_ui->edit_model_dc, &QLineEdit::editingFinished, this, &Window::slot_edit_model);
	QObject::connect(m_ui->edit_model_Ec, &QLineEdit::editingFinished, this, &Window::slot_edit_model);
	QObject::connect(m_ui->edit_model_sr, &QLineEdit::editingFinished, this, &Window::slot_edit_model);
}

//slots
void Window::slot_edit_model(void)
{
	bool test = false;
	double* values[] = {
		&m_tensegrity.m_Hc, &m_tensegrity.m_Ht, &m_tensegrity.m_Rr,
		&m_tensegrity.m_er, &m_tensegrity.m_tr, &m_tensegrity.m_tl,
		&m_tensegrity.m_dc, &m_tensegrity.m_Ec, &m_tensegrity.m_sr
	};
	QLineEdit* edits[] = {
		m_ui->edit_model_Hc, m_ui->edit_model_Ht, m_ui->edit_model_Rr,
		m_ui->edit_model_er, m_ui->edit_model_tr, m_ui->edit_model_tl,
		m_ui->edit_model_dc, m_ui->edit_model_Ec, m_ui->edit_model_sr
	};
	for(unsigned i = 0; i < sizeof(edits) / sizeof(QLineEdit*); i++)
	{
		if(QObject::sender() == edits[i])
		{
			const double r = *values[i];
			const double v = edits[i]->text().toDouble(&test);
			if(test)
			{
				*values[i] = v;
				edits[i]->setText(QString::asprintf("%+.2e", v));
			}
			else
			{
				edits[i]->setText(QString::asprintf("%+.2e", r));
			}
		}
	}
}