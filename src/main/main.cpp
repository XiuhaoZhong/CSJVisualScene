#include "mainwindow.h"

#include <QApplication>
#include <QDebug>

#include "Utils/CSJPathTool.h"

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

    CSJPathTool *pathTool = CSJPathTool::getInstance();
    pathTool->setWorkDirectory(fs::canonical(fs::path(argv[0]).remove_filename()));

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
