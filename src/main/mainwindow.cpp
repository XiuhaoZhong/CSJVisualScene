#include "mainwindow.h"

#include <QDebug>
#include <QPushButton>
#include <QHBoxLayout>

#include "Utils/CSJPathTool.h"
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
    setMinimumSize(1080, 720);

    CSJSceneEditorWindow* sceneRenderWindow = new CSJSceneEditorWindow(this);
    this->setCentralWidget(sceneRenderWindow);
}
