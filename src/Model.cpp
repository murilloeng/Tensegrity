//qt
#include <QtGui/QKeyEvent>
#include <QtWidgets/QGridLayout>

//canvas
#include "Canvas/inc/Scene/Scene.hpp"

//tensegrity
#include "Tensegrity/inc/Model.hpp"
#include "Tensegrity/inc/Canvas.hpp"

//constructors
Model::Model(const Tensegrity* tensegrity) : m_tensegrity(tensegrity)
{
	//data
	m_canvas = new Canvas(this);
	QWidget* layout = new QWidget(this);
	QGridLayout* grid_layout = new QGridLayout(layout);
	//canvas
	grid_layout->addWidget(m_canvas, 0, 0);
	//central
	setCentralWidget(layout);
}

//destructor
Model::~Model(void)
{
	return;
}

//update
void Model::update(void)
{
	m_tensegrity->draw_model(m_canvas->scene());
	m_canvas->scene()->update(true);
	m_canvas->update();
}

//events
void Model::keyPressEvent(QKeyEvent* event)
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