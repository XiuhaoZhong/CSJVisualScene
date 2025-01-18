#ifndef __CSJGLSHODAOWSCENE_H__
#define __CSJGLSHODAOWSCENE_H__

#include "CSJGLRender/CSJGLSceneBase.h"

#include <array>

class CSJRenderEntity {
    
};

class CSJGLShadowScene : public CSJGLSceneBase {
public:
    CSJGLShadowScene();
    ~CSJGLShadowScene();

    bool initScene(int width, int height) override;
    void drawScene(double tick) override;
    void resizeScene(int width, int height) override;
    void releaseScene() override;

private:

    // QOpenGLFramebufferObject    *m_shadowFramebuffer = nullptr;
    // QOpenGLTexture              *m_shadowTex;
    // QOpenGLShaderProgram         m_shadowProgram;

    // std::array<CSJSpGLBuffer, 4> m_bufferArray;
    // CSJSpGLBuffer                m_torusIndexBuffer;    

    // QOpenGLShaderProgram         m_renderProgram;

    // shape positions;
    glm::vec3 m_torusPos;
    glm::vec3 m_pyramidPos;
    glm::vec3 m_lightPos;
    glm::vec3 m_cameraPos;

    glm::vec3 m_originPos;
    glm::vec3 m_lightUpDirection;

    glm::mat4 m_projMat;
};

#endif // __CSJGLSHODAOWSCENE_H__