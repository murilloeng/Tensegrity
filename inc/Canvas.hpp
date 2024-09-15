#pragma once

//qt
#include "QtOpenGLWidgets/QOpenGLWidget"

class Model;
class Deformed;
namespace canvas
{
	class Scene;
}

class Canvas : public QOpenGLWidget
{
public:
	//constructor
	Canvas(QWidget*);

	//destructor
	~Canvas(void);

	//data
	canvas::Scene* scene(void) const;

private:
	//OpenGL
	void paintGL(void) override;
	void resizeGL(int, int) override;
	void initializeGL(void) override;

	//events
	void wheelEvent(QWheelEvent*) override;
	void keyPressEvent(QKeyEvent*) override;
	void mouseMoveEvent(QMouseEvent*) override;
	void mousePressEvent(QMouseEvent*) override;
	void mouseReleaseEvent(QMouseEvent*) override;

	//data
	canvas::Scene* m_scene;

	//friends
	friend class Model;
	friend class Deformed;
};