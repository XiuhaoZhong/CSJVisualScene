#ifndef __CSJWRAPPERWIDGET_H__
#define __CSJWRAPPERWIDGET_H__

#include <QWidget>

class CSJWrapperWidget : public QWidget {
    Q_OBJECT
public:
    CSJWrapperWidget(QWidget *parent = nullptr);
    ~CSJWrapperWidget();

protected:
    void initRenderWindow();
};

#endif // __CSJWRAPPERWIDGET_H__