#ifndef RENDERONSCREEN_H
#define RENDERONSCREEN_H

#include <QWidget>

namespace Ui {
class RenderOnScreen;
}

class RenderOnScreen : public QWidget
{
    Q_OBJECT

public:
    explicit RenderOnScreen(QWidget *parent = 0);
    ~RenderOnScreen();

private:
    Ui::RenderOnScreen *ui;
};

#endif // RENDERONSCREEN_H
