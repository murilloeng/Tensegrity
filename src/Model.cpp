//qt
#include <QtWidgets/QGridLayout>

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