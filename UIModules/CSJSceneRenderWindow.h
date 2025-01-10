#ifndef __CSJSCENERENDERWINDOW_H__
#define __CSJSCENERENDERWINDOW_H__

#include <memory>

#include <QWidget>

class CSJSceneRenderWindow : public QWidget {
    Q_OBJECT
public:
    CSJSceneRenderWindow(QWidget* parent = nullptr);
    ~CSJSceneRenderWindow();

    static std::shared_ptr<CSJSceneRenderWindow> createSceneRenderWindow();

    void initUI();

};

using CSJSpSceneRenderWindow = std::shared_ptr<CSJSceneRenderWindow>;



#endif // __CSJSCENERENDERWINDOW_H__