//qt
#include <QtGui/QKeyEvent>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QGridLayout>

//canvas
#include "Canvas/inc/Scene/Scene.hpp"

//tensegrity
#include "Tensegrity/inc/Canvas.hpp"
#include "Tensegrity/inc/Deformed.hpp"

//constructors
Deformed::Deformed(const Tensegrity* tensegrity) : m_tensegrity(tensegrity)
{
	//data
	m_canvas = new Canvas(this);
	QWidget* layout = new QWidget(this);
	QGridLayout* grid_layout = new QGridLayout(layout);
	QSlider* slider = new QSlider(Qt::Orientation::Horizontal, this);
	//canvas
	grid_layout->addWidget(slider, 0, 0);
	grid_layout->addWidget(m_canvas, 1, 0);
	//central
	setCentralWidget(layout);
}

//destructor
Deformed::~Deformed(void)
{
	return;
}

//update
void Deformed::update(void)
{
	m_tensegrity->draw_deformed(m_canvas->scene());
	m_canvas->scene()->update(true);
	m_canvas->update();
}

//events
void Deformed::keyPressEvent(QKeyEvent* event)
{
	if(event->key() == Qt::Key_Escape)
	{
		close();
	}
	else
	{
		m_canvas->keyPressEvent(event);
	}
}