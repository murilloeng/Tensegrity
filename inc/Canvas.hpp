#pragma once

//canvas
#include "Canvas/inc/Scene/Scene.hpp"

//qt
#include <QtOpenGLWidgets/QOpenGLWidget>

class Tensegrity;

class Canvas : public QOpenGLWidget
{
public:
	//constructors
	Canvas(QWidget*);

	//destructor
	~Canvas(void);

	//openGL
	void paintGL(void) override;
	void resizeGL(int, int) override;
	void initializeGL(void) override;

	//events
	void enterEvent(QEnterEvent*) override;
	void wheelEvent(QWheelEvent*) override;
	void keyPressEvent(QKeyEvent*) override;
	void mouseMoveEvent(QMouseEvent*) override;
	void mousePressEvent(QMouseEvent*) override;
	void mouseReleaseEvent(QMouseEvent*) override;

	//data
	canvas::Scene* m_scene;
	Tensegrity* m_tensegrity;
};