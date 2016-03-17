#ifndef SCROLLLABELEX_H
#define SCROLLLABELEX_H

#include <QWidget>
#include <QTimer>
#include <QString>

namespace Ui {
class ScrollLabelEx;
}

class ScrollLabelEx : public QWidget
{
    Q_OBJECT

public:
    ScrollLabelEx(QWidget *parent = 0);

    bool IsNeedScroll() const;

    void SetAnimationInterval(uint time) { m_intervalTime = time; }
    uint GetAnimationInterval() const { return m_intervalTime; }

    void SetPixmapsPerFrame(int length) { m_pixmapPerFrame = length; }
    int GetPixmapsPerFrame() const { return m_pixmapPerFrame; }

    void SetText(const QString& text);
    QString Text() const;

    void SetTextLabelStyleSheet(const QString& style);

    void SetPauseTime(uint time) { m_pauseTime = time; }
    uint GetPauseTime() const { return m_pauseTime; }

protected:
    virtual void resizeEvent(QResizeEvent *);

private:
    void StartScroll();

private:
    Ui::ScrollLabelEx         *ui;

    // scroll speed
    QTimer                  m_timer;
    QTimer                  m_pauseTimer;
    uint                    m_intervalTime;
    uint                    m_frame;
    int                     m_pixmapPerFrame;

    uint                    m_pauseTime;

private slots:
    void OnTimeOut();
    void OnPauseTimeOut();
};

#endif
