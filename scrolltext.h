#ifndef SCROLLTEXT_H
#define SCROLLTEXT_H

#include <QWidget>
#include <QStaticText>
#include <QTimer>
#include <QFont>


class ScrollText : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QString separator READ separator WRITE setSeparator)

public:
    explicit ScrollText(QWidget *parent = 0);

public slots:
    QString text() const;
    void setText(QString text);

    QString separator() const;
    void setSeparator(QString separator);

    uint waitForStartTime() const;
    void setWaitForStartTime(uint time);

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);

private:
    void updateText();
    QString _text;
    QString _separator;
    QStaticText staticText;
    int singleTextWidth;
    QSize wholeTextSize;
    int leftMargin;
    bool scrollEnabled;
    int scrollPos;
    QImage alphaChannel;
    QImage buffer;
    QTimer timer;

    // append
    uint        m_waitForStartTime;

private slots:
    virtual void timer_timeout();
};

#endif // SCROLLTEXT_H
