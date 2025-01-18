#include "CSJGLShadowScene.h"

CSJGLShadowScene::CSJGLShadowScene() {
    
    m_torusPos   = glm::vec3(1.6f, 0.0f, -0.3f);
    m_pyramidPos = glm::vec3(-1.0f, 0.1f, 0.3f);
    m_cameraPos  = glm::vec3(0.0f, 0.2f, 6.0f);
    m_lightPos   = glm::vec3(-3.8f, 2.2f, 1.1f);
    m_originPos  = glm::vec3(0.0f, 0.0f, 0.0f);
    m_lightUpDirection = glm::vec3(0.0f, 1.0f, 0.0f);
}

CSJGLShadowScene::~CSJGLShadowScene() {

}

bool CSJGLShadowScene::initScene(int width, int height) {


    float aspect = (float)width / float(height);
    m_projMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

    return false;
}

void CSJGLShadowScene::drawScene(double tick) {

}

void CSJGLShadowScene::resizeScene(int width, int height) {

}

void CSJGLShadowScene::releaseScene() {

}
