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

	//data
	Ui::Window* m_ui;
	Tensegrity m_tensegrity;
};