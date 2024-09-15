#pragma once

//qt
#include "QtWidgets/QMainWindow"

//tensegrity
#include "Tensegrity/inc/Canvas.hpp"
#include "Tensegrity/inc/Tensegrity.hpp"

class Deformed : public QMainWindow
{
public:
	//constructors
	Deformed(const Tensegrity*);

	//destructor
	~Deformed(void);

	//update
	void update(void);

private:
	//events
	void keyPressEvent(QKeyEvent*) override;

	//data
	Canvas* m_canvas;
	const Tensegrity* m_tensegrity;
};