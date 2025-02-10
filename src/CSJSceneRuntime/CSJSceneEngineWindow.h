#ifndef __CSJSCENEENGINEWINDOW_H__
#define __CSJSCENEENGINEWINDOW_H__

#include <QVulkanWindow>

class CSJSceneEngineWindow : public QVulkanWindow {
     Q_OBJECT
public:
    CSJSceneEngineWindow(QWindow *parent = nullptr);
    ~CSJSceneEngineWindow();

    QVulkanWindowRenderer *createRenderer() override;
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
};

#endif // __CSJSCENEENGINEWINDOW_H__