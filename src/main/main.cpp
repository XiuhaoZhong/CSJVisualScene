#include "mainwindow.h"

#include <iostream>
#include <locale>

#include <QApplication>
#include <QDebug>
#include <QString>

#include "Utils/CSJPathTool.h"
#include "Utils/CSJStringTool.h"
#include "Utils/CSJLogger.h"

#include "CSJSceneRuntime/CSJSceneEngineWindow.h"
#include "CSJSceneRuntime/CSJSceneRuntimeData.h"

void runGameSceneDirectly();

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    CSJLogger *logger = CSJLogger::getLoggerInst();

    QVulkanInstance inst;
    if (!inst.create()) {
        logger->log_fatal("QVulkanInstance create failed!");
        return -1;
    }

    CSJPathTool *pathTool = CSJPathTool::getInstance();

    std::string path_str(argv[0]);
    pathTool->setWorkDirectory(fs::canonical(fs::path(argv[0]).remove_filename()));

    CSJSceneRumtimeData::setVulkanInstance(&inst);

    logger->log_info("Vulkan intance create successfully!");

    MainWindow w;
    w.show();
    return a.exec();
}

void runGameSceneDirectly() {
    CSJSceneEngineWindow *window = new CSJSceneEngineWindow();

    window->resize(1080, 760);
    window->show();
}
