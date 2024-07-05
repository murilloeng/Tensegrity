#pragma once

//qt
#include <QtWidgets/QMainWindow>

//tensegrity
#include "Tensegrity/inc/Tensegrity.hpp"

namespace Ui
{
	class Window;
}

class Window : public QMainWindow
{
public:
	//constructor
	Window(void);

	//destructor
	~Window(void);

private:
	//setup
	void setup(void);
	void connect(void);

private slots:
	//slots
	void slot_edit_model(void);

public:
	//data
	Ui::Window* m_ui;
	Tensegrity m_tensegrity;
};