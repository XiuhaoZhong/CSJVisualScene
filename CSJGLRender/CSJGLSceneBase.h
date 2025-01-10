#ifndef __CSJGLSCENEBASE_H__
#define __CSJGLSCENEBASE_H__

#include "render/CSJRenderObjectData.h"

#include <QSharedPointer>

#include <QOpenGLWidget>

#include <QOpenGLTexture>
#include <QOpenGLFrameBufferObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

using CSJSpGLBuffer = QSharedPointer<QOpenGLBuffer>;

class CSJGLSceneBase {
public:
    CSJGLSceneBase(QOpenGLWidget *contextWidget);
    ~CSJGLSceneBase();

    virtual bool initScene(int width, int height) = 0;
    virtual void drawScene(double tick) = 0;
    virtual void resizeScene(int width, int height) = 0;

    virtual void releaseScene() = 0;

protected:
    QOpenGLWidget *m_pContextWidget;
};

#endif // __CSJGLSCENE_H__