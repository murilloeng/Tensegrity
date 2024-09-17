#pragma once

//qt
#include "QtWidgets/QWidget"

//tensegrity
#include "Tensegrity/inc/Canvas.hpp"
#include "Tensegrity/inc/Tensegrity.hpp"

class Model : public QWidget
{
public:
	//constructors
	Model(QWidget*, const Tensegrity*);

	//destructor
	~Model(void);

	//update
	void update(void);

private:
	//events
	void keyPressEvent(QKeyEvent*) override;

	//data
	Canvas* m_canvas;
	const Tensegrity* m_tensegrity;
};