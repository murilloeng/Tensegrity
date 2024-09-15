#pragma once

//qt
#include "QtWidgets/QMainWindow"

//tensegrity
#include "Tensegrity/inc/Canvas.hpp"
#include "Tensegrity/inc/Tensegrity.hpp"

class Model : public QMainWindow
{
public:
	//constructors
	Model(const Tensegrity*);

	//destructor
	~Model(void);

private:
	Canvas* m_canvas;
	const Tensegrity* m_tensegrity;
};