#include "mainwindow.h"

#include <QPushButton>
#include <QHBoxLayout>

#include "CSJSceneEditor/CSJSceneEditorWindow.h"

#include "CSJSceneRuntime/CSJSceneEngine.h"
#include "CSJSceneRuntime/CSJSceneEngineWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    initUI();
}

MainWindow::~MainWindow() {

}

void MainWindow::initUI() {
    setFixedSize(1080, 720);
    
    // CSJSceneEngineWindow *engineWindow = new CSJSceneEngineWindow();
    // QWidget *renderWidget = QWidget::createWindowContainer(engineWindow);
    // QLayout *layout = this->layout();//new QHBoxLayout(this);
    // layout->addWidget(renderWidget);

    QPushButton *sceneButton = new QPushButton(this);
    sceneButton->setText("Open a Scene");
    connect(sceneButton, SIGNAL(clicked()), this, SLOT(showSceneRenderWindow()));
}

void MainWindow::showSceneRenderWindow() {
    CSJSceneEditorWindow* sceneRenderWindow = new CSJSceneEditorWindow();
    sceneRenderWindow->show();
}
