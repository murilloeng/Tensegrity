#pragma once

//qt
#include "QtOpenGLWidgets/QOpenGLWidget"

class Canvas : public QOpenGLWidget
{
public:
	//constructor
	Canvas(QWidget*);

	//destructor
	~Canvas(void);

private:
	//OpenGL
	void paintGL(void) override;
	void resizeGL(int, int) override;
	void initializeGL(void) override;
};