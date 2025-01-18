#include "CSJSceneEditorWindow.h"

#include <QApplication>
#include <QScreen>
#include <QSplitter>
#include <QHBoxLayout>

#include "CSJWrapperWidget.h"


// #include "CSJSceneRuntime/CSJSceneEngine.h"
// #include "CSJSceneRuntime/CSJSceneEngineWindow.h"

CSJSpSceneRenderWindow CSJSceneEditorWindow::createSceneRenderWindow() {
    return std::make_shared<CSJSceneEditorWindow>();
}

CSJSceneEditorWindow::CSJSceneEditorWindow(QWidget* parent) 
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

void CSJSceneEditorWindow::initUI() {
    setMinimumSize(1080, 720);

    QHBoxLayout *boxLayout = new QHBoxLayout(this);

    QSplitter *leftSplitter = new QSplitter(this); 
    boxLayout->addWidget(leftSplitter);

    QWidget *leftWidget = new QWidget();   
    leftSplitter->addWidget(leftWidget);

    // QVulkanInstance inst;
    // if (!inst.create()) {
    //     qDebug() << "QVulkanInstance create failed!";
    // } else {
    //     qDebug() << "QVulkanInstance create success!";
    //     CSJSceneEngineWindow *engineWindow = new CSJSceneEngineWindow();
    //     engineWindow->setVulkanInstance(&inst);
    //     QWidget *renderWidget = QWidget::createWindowContainer(engineWindow);
    //     renderWidget->setMinimumSize({300, 400});
    //     leftSplitter->addWidget(renderWidget);
    // }

    CSJWrapperWidget *wrapperWidget = new CSJWrapperWidget(this);
    leftSplitter->addWidget(wrapperWidget);

    QWidget *rightWidget = new QWidget();   
    leftSplitter->addWidget(rightWidget);
}

CSJSceneEditorWindow::~CSJSceneEditorWindow() {

}

