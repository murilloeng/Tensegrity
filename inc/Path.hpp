#pragma once

//qt
#include "QtWidgets/QWidget"

//tensegrity
#include "Tensegrity/inc/Tensegrity.hpp"

class Path : public QWidget
{
public:
	//constructors
	Path(QWidget*, const Tensegrity*);

	//destructor
	~Path(void);

private:
	//data
	const Tensegrity* m_tensegrity;
};