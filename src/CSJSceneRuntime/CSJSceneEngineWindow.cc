#include "CSJSceneEngineWindow.h"

#include "Utils/CSJLogger.h"

#include "CSJSceneEngine.h"
#include "CSJSceneRuntimeData.h"

CSJSceneEngineWindow::CSJSceneEngineWindow(QWindow *parent)
    : QVulkanWindow(parent)
    , m_pLogger(CSJLogger::getLoggerInst()) {
    setVulkanInstance(CSJSceneRumtimeData::getVulkanInstance());
}

CSJSceneEngineWindow::~CSJSceneEngineWindow() {

}

QVulkanWindowRenderer *CSJSceneEngineWindow::createRenderer() {
    return new CSJSceneEngine(this);
}

void CSJSceneEngineWindow::mousePressEvent(QMouseEvent *event) {
    m_pLogger->log_info("CSJSceneEngineWindow::mousePressEvent!");
    
}

void CSJSceneEngineWindow::mouseReleaseEvent(QMouseEvent *event) {
    m_pLogger->log_info("CSJSceneEngineWindow::mouseReleaseEvent!");
}

void CSJSceneEngineWindow::keyPressEvent(QKeyEvent *event) {
    m_pLogger->log_info("CSJSceneEngineWindow::keyPressEvent!");
}

void CSJSceneEngineWindow::keyReleaseEvent(QKeyEvent *event) {
    m_pLogger->log_info("CSJSceneEngineWindow::keyReleaseEvent!"); 
}

void CSJSceneEngineWindow::wheelEvent(QWheelEvent *event) {
    m_pLogger->log_info("CSJSceneEngineWindow::wheelEvent!");
}
