#include "scrolllabelex.h"
#include "ui_scrolllabelex.h"

#include <QResizeEvent>

ScrollLabelEx::ScrollLabelEx(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ScrollLabelEx)
    , m_timer(this)
    , m_pauseTimer(this)
    , m_intervalTime(100)
    , m_frame(0)
    , m_pixmapPerFrame(-2)
    , m_pauseTime(2000)
{
    ui->setupUi(this);

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(OnTimeOut()));
    connect(&m_pauseTimer, SIGNAL(timeout()), this, SLOT(OnPauseTimeOut()));
    m_timer.setInterval(m_intervalTime);
    m_pauseTimer.setInterval(m_pauseTime);
    m_pauseTimer.setSingleShot(true);
}

bool ScrollLabelEx::IsNeedScroll() const
{
    const QString text = ui->textlabel->text();
    const int textWidth = fontMetrics().width(text);
    const int labelWidth = width();

    return (textWidth > labelWidth);
}

void ScrollLabelEx::resizeEvent(QResizeEvent * e)
{
    ui->containwidget->resize(e->size());
    ui->textlabel->resize(ui->textlabel->width(), ui->containwidget->size().height());
}

void ScrollLabelEx::StartScroll()
{
    if(IsNeedScroll())
        m_timer.start();
}

void ScrollLabelEx::SetText(const QString& text)
{
    ui->textlabel->setText(text);
    StartScroll();
}

QString ScrollLabelEx::Text() const
{
    return ui->textlabel->text();
}

void ScrollLabelEx::OnTimeOut()
{
    const QString text = ui->textlabel->text();
    const uint textWidth = ui->textlabel->fontMetrics().width(text);

    if( qAbs((m_frame + 1) * qAbs(m_pixmapPerFrame)) > textWidth ) {
        m_frame = 0;
        m_timer.stop();
        m_pauseTimer.start();
    } else {
        m_frame++;
    }

    ui->textlabel->move(m_frame * m_pixmapPerFrame, 0);
}

void ScrollLabelEx::SetTextLabelStyleSheet(const QString& style)
{
    ui->textlabel->setStyleSheet(style);
}

void ScrollLabelEx::OnPauseTimeOut()
{
    StartScroll();
}
