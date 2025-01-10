#include "mainwindow.h"

#include <QPushButton>

#include "UIModules/CSJSceneRenderWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    initUI();
}

MainWindow::~MainWindow() {

}

void MainWindow::initUI() {
    setFixedSize(1280, 960);

    QPushButton *sceneButton = new QPushButton(this);
    sceneButton->setText("Open a Scene");
    connect(sceneButton, SIGNAL(clicked()), this, SLOT(showSceneRenderWindow()));
}

void MainWindow::showSceneRenderWindow() {
    CSJSceneRenderWindow* sceneRenderWindow = new CSJSceneRenderWindow();
    sceneRenderWindow->show();
}
