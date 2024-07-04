//qt
#include <QtGui/QKeyEvent>
#include <QtGui/QWheelEvent>

//tensegrity
#include "Tensegrity/inc/Canvas.hpp"
#include "Tensegrity/inc/Tensegrity.hpp"

//constructors
Canvas::Canvas(QWidget* parent) : QOpenGLWidget(parent), m_scene(nullptr)
{
	return;
}

//destructor
Canvas::~Canvas(void)
{
	delete m_scene;
}

//openGL
void Canvas::paintGL(void)
{
	m_scene->draw();
}
void Canvas::resizeGL(int width, int height)
{
	m_scene->camera().callback_reshape(width, height);
	m_scene->update(false);
}
void Canvas::initializeGL(void)
{
	//glew
	if(glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Error: can't setup glew!\n");
		exit(EXIT_FAILURE);
	}
	//scene
	glLineWidth(2);
	m_scene = new ::canvas::Scene("../Canvas/shd/");
	//tensegrity
	m_tensegrity->draw_model(m_scene);
	m_scene->update(true);
}

//events
void Canvas::enterEvent(QEnterEvent* event)
{
	setFocus();
}
void Canvas::wheelEvent(QWheelEvent* event)
{
	//event
	const unsigned x1 = event->position().x();
	const unsigned x2 = event->position().y();
	const unsigned direction = event->angleDelta().y();
	m_scene->camera().callback_wheel(direction, x1, x2);
	//update
	update();
}
void Canvas::keyPressEvent(QKeyEvent* event)
{
	//data
	const unsigned key = event->key();
	const char ckn[] = {'f', 'i', 'x', 'y', 'z', '-', '+'};
	const Qt::Key qks[] = {Qt::Key_Left, Qt::Key_Right, Qt::Key_Down, Qt::Key_Up};
	const Qt::Key qkn[] = {Qt::Key_F, Qt::Key_I, Qt::Key_X, Qt::Key_Y, Qt::Key_Z, Qt::Key_Minus, Qt::Key_Plus};
	const ::canvas::key cks[] = {::canvas::key::left, ::canvas::key::right, ::canvas::key::down, ::canvas::key::up};
	//modifiers
	unsigned modifiers = 0;
	if(event->modifiers() & Qt::AltModifier) modifiers |= 1 << unsigned(::canvas::modifier::alt);
	if(event->modifiers() & Qt::ShiftModifier) modifiers |= 1 << unsigned(::canvas::modifier::shift);
	if(event->modifiers() & Qt::ControlModifier) modifiers |= 1 << unsigned(::canvas::modifier::ctrl);
	//normal keys
	for(unsigned i = 0; i < sizeof(qkn) / sizeof(Qt::Key); i++)
	{
		if(event->key() == qkn[i])
		{
			m_scene->camera().callback_keyboard(ckn[i]);
		}
	}
	//special keys
	for(unsigned i = 0; i < sizeof(qks) / sizeof(Qt::Key); i++)
	{
		if(event->key() == qks[i])
		{
			m_scene->camera().callback_special(cks[i], modifiers, 0, 0);
		}
	}
	//update
	update();
}
void Canvas::mouseMoveEvent(QMouseEvent* event)
{
	//event
	const unsigned x1 = event->position().x();
	const unsigned x2 = event->position().y();
	m_scene->camera().callback_motion(x1, x2);
	//update
	update();
}
void Canvas::mousePressEvent(QMouseEvent* event)
{
	//data
	const Qt::MouseButton qt_buttons[] = {
		Qt::MouseButton::LeftButton, 
		Qt::MouseButton::RightButton, 
		Qt::MouseButton::MiddleButton
	};
	const Qt::CursorShape qt_cursors[] = {
		Qt::CursorShape::CrossCursor,
		Qt::CursorShape::DragMoveCursor,
		Qt::CursorShape::ClosedHandCursor
	};
	const ::canvas::button canvas_buttons[] = {
		::canvas::button::left, 
		::canvas::button::right, 
		::canvas::button::middle
	};
	const unsigned x1 = event->position().x();
	const unsigned x2 = event->position().y();
	//callback
	for(unsigned i = 0; i < 3; i++)
	{
		if(event->button() & qt_buttons[i])
		{
			setCursor(QCursor(qt_cursors[i]));
			m_scene->camera().callback_mouse(canvas_buttons[i], true, x1, x2);
		}
	}
	//update
	update();
}
void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
	//data
	const ::canvas::button canvas_buttons[] = {
		::canvas::button::left, 
		::canvas::button::right, 
		::canvas::button::middle
	};
	const Qt::MouseButton qt_buttons[] = {
		Qt::MouseButton::LeftButton, 
		Qt::MouseButton::RightButton, 
		Qt::MouseButton::MiddleButton
	};
	const unsigned x1 = event->position().x();
	const unsigned x2 = event->position().y();
	//callback
	for(unsigned i = 0; i < 3; i++)
	{
		if(event->button() & qt_buttons[i])
		{
			setCursor(QCursor(Qt::CursorShape::ArrowCursor));
			m_scene->camera().callback_mouse(canvas_buttons[i], false, x1, x2);
		}
	}
	//update
	update();
}