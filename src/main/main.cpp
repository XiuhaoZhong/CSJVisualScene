#include "mainwindow.h"

#include <iostream>

#include <QApplication>
#include <QDebug>
#include <QString>

#include "Utils/CSJPathTool.h"
#include "Utils/CSJStringTool.h"

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

    std::string path_str(argv[0]);
    std::cout << "Working path: " << path_str << std::endl;
    std::wstring path_wstr = CSJStringTool::string2wstring(path_str);
    //qDebug() << "The execution path: " << QString::fromStdWString(path_wstr);
    std::wcout << L"The execution path wide version: " << path_wstr << std::endl;
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
