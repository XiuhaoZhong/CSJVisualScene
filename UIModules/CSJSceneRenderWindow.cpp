#include "CSJSceneRenderWindow.h"

#include <QApplication>
#include <QScreen>

#include "CSJGLRenderWidget.h"

CSJSpSceneRenderWindow CSJSceneRenderWindow::createSceneRenderWindow() {
    return std::make_shared<CSJSceneRenderWindow>();
}

CSJSceneRenderWindow::CSJSceneRenderWindow(QWidget* parent) 
    : QWidget(parent) {
    
    setAttribute(Qt::WA_DeleteOnClose, true);

    QPoint screenCenter = QApplication::screens().constFirst()->availableGeometry().center();
    QRect curRect = this->geometry();
    setGeometry(QRect(screenCenter.rx() - curRect.width() / 2,
                                       screenCenter.ry() - curRect.height() / 2,
                                       curRect.width(),
                                       curRect.height()));

    initUI();
}

void CSJSceneRenderWindow::initUI() {
    setFixedSize(800, 800);

    CSJGLRenderWidget *glRenderWidget = new CSJGLRenderWidget(this);
    glRenderWidget->show();
}

CSJSceneRenderWindow::~CSJSceneRenderWindow() {

}

