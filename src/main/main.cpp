#include "mainwindow.h"

#include <QApplication>
#include <QDebug>

#include "CSJSceneRuntime/CSJSceneEngineWindow.h"
#include "CSJSceneRuntime/CSJSceneRuntimeData.h"

void runGameSceneDirectly();

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QVulkanInstance inst;
    if (!inst.create()) {
        qDebug() << "QVulkanInstance create failed!";
        return -1;
    }

    CSJSceneRumtimeData::setVulkanInstance(&inst);

    MainWindow w;
    w.show();
    return a.exec();
}

void runGameSceneDirectly() {
    CSJSceneEngineWindow *window = new CSJSceneEngineWindow();

    window->resize(1080, 760);
    window->show();
}
