//qt
#include <QtGui/QKeyEvent>
#include <QtWidgets/QGridLayout>

//canvas
#include "Canvas/inc/Scene/Scene.hpp"

//tensegrity
#include "Tensegrity/inc/Model.hpp"
#include "Tensegrity/inc/Canvas.hpp"

//constructors
Model::Model(QWidget* parent, const Tensegrity* tensegrity) : QWidget(parent), m_tensegrity(tensegrity)
{
	//data
	m_canvas = new Canvas(this);
	QGridLayout* grid_layout = new QGridLayout(this);
	//canvas
	grid_layout->addWidget(m_canvas, 0, 0);
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
	m_canvas->keyPressEvent(event);
}