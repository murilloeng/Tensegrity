#pragma once

//qt
#include "QtWidgets/QMainWindow"

//tensegrity
#include "Tensegrity/inc/Path.hpp"
#include "Tensegrity/inc/Model.hpp"
#include "Tensegrity/inc/Deformed.hpp"
#include "Tensegrity/inc/Tensegrity.hpp"

class Window : public QMainWindow
{
public:
	//constructors
	Window(const Tensegrity*);

	//destructor
	~Window(void);

	//data
	Path* path(void);
	Model* model(void);
	Deformed* deformed(void);

private:
	//slots
	void slot_tabs(int32_t);

	//data
	Path* m_path;
	Model* m_model;
	Deformed* m_deformed;
	const Tensegrity* m_tensegrity;
};