#include "CSJSceneEngineWindow.h"

#include <QDebug>

#include "CSJSceneEngine.h"
#include "CSJSceneRuntimeData.h"

CSJSceneEngineWindow::CSJSceneEngineWindow(QWindow *parent)
    : QVulkanWindow(parent) {
    setVulkanInstance(CSJSceneRumtimeData::getVulkanInstance());
}

CSJSceneEngineWindow::~CSJSceneEngineWindow() {

}

QVulkanWindowRenderer *CSJSceneEngineWindow::createRenderer() {
    return new CSJSceneEngine(this);
}

void CSJSceneEngineWindow::mousePressEvent(QMouseEvent *event) {
    qDebug() << "CSJSceneEngineWindow::mousePressEvent!";
}

void CSJSceneEngineWindow::mouseReleaseEvent(QMouseEvent *event) {
    qDebug() << "CSJSceneEngineWindow::mouseReleaseEvent!";
}

void CSJSceneEngineWindow::keyPressEvent(QKeyEvent *event) {
    qDebug() << "CSJSceneEngineWindow::keyPressEvent!";
}

void CSJSceneEngineWindow::keyReleaseEvent(QKeyEvent *event) {
    qDebug() << "CSJSceneEngineWindow::keyReleaseEvent!"; 
}

void CSJSceneEngineWindow::wheelEvent(QWheelEvent *event) {
    qDebug() << "CSJSceneEngineWindow::wheelEvent!";
}
