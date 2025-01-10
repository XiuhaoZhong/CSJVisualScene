#ifndef __CSJGLRENDERWIDGET_H__
#define __CSJGLRENDERWIDGET_H__

#include <memory>

#include <QOpenGLWidget>

#include "CSJGLRender/CSJGLSceneBase.h"

class CSJGLRenderWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    CSJGLRenderWidget(QWidget* parent = nullptr);
    ~CSJGLRenderWidget();

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
};

using CSJSpGLRenderWidget = std::shared_ptr<CSJGLRenderWidget>;

#endif // __CSJGLRENDERWIDGET_H__