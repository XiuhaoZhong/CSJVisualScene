#include "CSJGLRenderWidget.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QDebug>

CSJGLRenderWidget::CSJGLRenderWidget(QWidget* parent)
    : QOpenGLWidget(parent) {
    setFixedSize(500, 500);
}

CSJGLRenderWidget::~CSJGLRenderWidget() {

}

void CSJGLRenderWidget::initializeGL() {
    QOpenGLFunctions *glFuncs = QOpenGLContext::currentContext()->functions();
    GLint major, minor;
    glFuncs->glGetIntegerv(GL_MAJOR_VERSION, &major);
    glFuncs->glGetIntegerv(GL_MINOR_VERSION, &minor);

    qDebug() << "OpenGL Version Info, Major: " << major << ", Minor: " << minor;

    glFuncs->glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
}

void CSJGLRenderWidget::paintGL() {
    QOpenGLFunctions *glFuncs = QOpenGLContext::currentContext()->functions();
    glFuncs->glClear(GL_COLOR_BUFFER_BIT);
}

void CSJGLRenderWidget::resizeGL(int w, int h) {

}