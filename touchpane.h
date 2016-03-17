#ifndef TOUCHPANE_H
#define TOUCHPANE_H

#include <QWidget>
#include <QTime>

namespace Ui {
class TouchPane;
}

class TouchPane : public QWidget
{
    Q_OBJECT

public:
    explicit TouchPane(QWidget *parent = 0);
    ~TouchPane();

#if 1
    virtual bool eventFilter(QObject *, QEvent *);
#endif
    void SetBackgroundTransparent(bool);

protected:
    virtual bool event(QEvent *);
    virtual void resizeEvent(QResizeEvent *);

private:
    void init();
    void final();

    void InstallEventFilter();
    void RemoveEventFilter();

    QString GetMouseSenderMsg(Qt::MouseButton btnIndex, const QPoint& pos, const QSize& size, bool mouseMoving = true);

    void ComponentsInit();
    void ComponentsGeometryAdjust();

    void HandleMouseEvent(QEvent *e);
    void HandleTouchEvent(QEvent *e);

private:
    Ui::TouchPane *ui;
    float          m_sensitivity;
    bool           m_touchhandle;

public slots:
    void HelpPaneVisible(bool);

signals:
    void MousePaneUpdate(const QString&);
};

#endif // TOUCHPANE_H
