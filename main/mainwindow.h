#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initUI();

protected slots:
    void showSceneRenderWindow();
};
#endif // MAINWINDOW_H
