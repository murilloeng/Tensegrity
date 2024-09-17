#pragma once

//qt
#include "QtWidgets/QLabel"
#include "QtWidgets/QSlider"
#include "QtWidgets/QWidget"

//tensegrity
#include "Tensegrity/inc/Canvas.hpp"
#include "Tensegrity/inc/Tensegrity.hpp"

class Deformed : public QWidget
{
public:
	//constructors
	Deformed(QWidget*, const Tensegrity*);

	//destructor
	~Deformed(void);

	//update
	void update(void);
	void update_label(void);
	void update_canvas(void);

private:
	//slots
	void slot_slider(void);

	//events
	void keyPressEvent(QKeyEvent*) override;

	//data
	QLabel* m_label;
	Canvas* m_canvas;
	QSlider* m_slider;
	const Tensegrity* m_tensegrity;
};