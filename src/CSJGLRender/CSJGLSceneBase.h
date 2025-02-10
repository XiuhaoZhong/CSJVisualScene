#ifndef __CSJGLSCENEBASE_H__
#define __CSJGLSCENEBASE_H__

#include "CSJSceneRuntime/function/framework/render/CSJRenderObjectData.h"

#include <QSharedPointer>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

class CSJGLSceneBase {
public:
    CSJGLSceneBase();
    ~CSJGLSceneBase();

    virtual bool initScene(int width, int height) = 0;
    virtual void drawScene(double tick) = 0;
    virtual void resizeScene(int width, int height) = 0;

    virtual void releaseScene() = 0;

protected:

};

#endif // __CSJGLSCENE_H__