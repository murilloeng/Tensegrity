//qt
#include <QtGui/QKeyEvent>
#include <QtWidgets/QGridLayout>

//tensegrity
#include "Tensegrity/inc/Path.hpp"

//constructors
Path::Path(QWidget* parent, const Tensegrity* tensegrity) : QWidget(parent), m_tensegrity(tensegrity)
{
	// //data
	// QWidget* layout = new QWidget(this);
	// QGridLayout* grid_layout = new QGridLayout(layout);
}

//destructor
Path::~Path(void)
{
	return;
}