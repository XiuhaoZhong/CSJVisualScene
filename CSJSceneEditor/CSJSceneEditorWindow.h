#ifndef __CSJSCENERENDERWINDOW_H__
#define __CSJSCENERENDERWINDOW_H__

#include <memory>

#include <QWidget>

class CSJSceneEditorWindow : public QWidget {
    Q_OBJECT
public:
    CSJSceneEditorWindow(QWidget* parent = nullptr);
    ~CSJSceneEditorWindow();

    static std::shared_ptr<CSJSceneEditorWindow> createSceneRenderWindow();

    void initUI();

};

using CSJSpSceneRenderWindow = std::shared_ptr<CSJSceneEditorWindow>;

#endif // __CSJSCENERENDERWINDOW_H__