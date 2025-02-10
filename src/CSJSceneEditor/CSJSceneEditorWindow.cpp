#include "CSJSceneEditorWindow.h"

#include <QApplication>
#include <QScreen>
#include <QSplitter>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "CSJSceneRuntime/CSJSceneEngine.h"
#include "CSJSceneRuntime/CSJSceneEngineWindow.h"

CSJSpSceneRenderWindow CSJSceneEditorWindow::createSceneRenderWindow() {
    return std::make_shared<CSJSceneEditorWindow>();
}

CSJSceneEditorWindow::CSJSceneEditorWindow(QWidget* parent) 
    : QWidget(parent) {
    
    if (!parent) {
        setAttribute(Qt::WA_DeleteOnClose, true);
        QPoint screenCenter = QApplication::screens().constFirst()->availableGeometry().center();
        QRect curRect = this->geometry();
        setGeometry(QRect(screenCenter.rx() - curRect.width() / 2,
                    screenCenter.ry() - curRect.height() / 2,
                    curRect.width(),
                    curRect.height()));
                
        setMinimumSize(1080, 720);
    }

    initUI();
}

void CSJSceneEditorWindow::initUI() {
    QHBoxLayout *boxLayout = new QHBoxLayout(this);

    QWidget *leftWidgget = new QWidget();   
    boxLayout->addWidget(leftWidgget);
    leftWidgget->setMinimumWidth(100);
    leftWidgget->setMaximumWidth(200);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    boxLayout->addLayout(rightLayout);

    CSJSceneEngineWindow *engineWindow = new CSJSceneEngineWindow();
    QWidget *renderWidget = QWidget::createWindowContainer(engineWindow, this, 
                                                            Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    renderWidget->setMinimumSize({400, 300});
    rightLayout->addWidget(renderWidget);

    QWidget *logWidget = new QWidget(this);
    logWidget->setMinimumHeight(100);
    logWidget->setMaximumHeight(200);
    rightLayout->addWidget(logWidget);
}

CSJSceneEditorWindow::~CSJSceneEditorWindow() {

}

