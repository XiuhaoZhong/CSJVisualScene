#include "CSJWrapperWidget.h"

#include <QDebug>
#include <QHBoxLayout>

#include "CSJSceneRuntime/CSJSceneEngine.h"
#include "CSJSceneRuntime/CSJSceneEngineWindow.h"

CSJWrapperWidget::CSJWrapperWidget(QWidget *parent) 
    : QWidget(parent) {

    setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setMinimumSize(300, 400);
    initRenderWindow();
}

CSJWrapperWidget::~CSJWrapperWidget() {

}

void CSJWrapperWidget::initRenderWindow() {
    // QVulkanInstance inst;
    // if (!inst.create()) {
    //     qDebug() << "QVulkanInstance create failed!";
    // } else {
    //     qDebug() << "QVulkanInstance create success!";
    //     CSJSceneEngineWindow *engineWindow = new CSJSceneEngineWindow();
    //     //engineWindow->show();
    //     engineWindow->setVulkanInstance(&inst);
    //     QWidget *renderWidget = QWidget::createWindowContainer(engineWindow, this, 
    //                                                            Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    //     QHBoxLayout *layout = new QHBoxLayout(this);
    //     layout->addWidget(renderWidget);
    //     //renderWidget->setParent(this);
    //     renderWidget->setMinimumSize({300, 400});
    // }

    CSJSceneEngineWindow *engineWindow = new CSJSceneEngineWindow();
    QWidget *renderWidget = QWidget::createWindowContainer(engineWindow, this, 
                                                            Qt::CustomizeWindowHint | Qt::FramelessWindowHint);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(renderWidget);
    renderWidget->setMinimumSize({300, 400});
}
