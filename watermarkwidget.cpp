#include "watermarkwidget.h"
#include "ui_watermarkwidget.h"

#include <QPaintEvent>
#include <QMouseEvent>
#include <QFont>
#include <QColor>
#include <QString>
#include <QPainter>
#include <QFontMetrics>
#include <QDebug>

WaterMarkWidget::WaterMarkWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WaterMarkWidget)
{
    ui->setupUi(this);
    resize(1920, 1080);
}

WaterMarkWidget::~WaterMarkWidget()
{
    delete ui;
}

void WaterMarkWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QFontMetrics fontMetrics(m_font);
    QRect rect = fontMetrics.boundingRect(QRect(0, 0, 1920, 1080), Qt::AlignLeft, m_text);
    resize(rect.size());
    QBrush brush(QColor(0xff, 0xff, 0xff, 0x40));
    painter.setPen(Qt::transparent);
    painter.setBrush(brush);
    painter.drawRect(rect);
    painter.setFont(m_font);
    painter.setPen(QPen(m_color));
    painter.drawText(rect, Qt::AlignLeft, m_text);
}

void WaterMarkWidget::mousePressEvent(QMouseEvent *mouseEvent)
{
    m_lastGloablPoint = mouseEvent->globalPos();
    m_lastPoint = pos();
}

void WaterMarkWidget::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    move(mouseEvent->globalPos() - m_lastGloablPoint + m_lastPoint);
}

void WaterMarkWidget::fontFamilyChanged(const QFont& font)
{
    m_font = font;
    repaint();
}

void WaterMarkWidget::fontSizeChanged(uint size)
{
    m_font.setPixelSize(size);
    repaint();
}

void WaterMarkWidget::BoldStateChanged(bool state)
{
    m_font.setBold(state);
    repaint();
}

void WaterMarkWidget::ItalyStateChanged(bool state)
{
    m_font.setItalic(state);
    repaint();
}

void WaterMarkWidget::TextColorChanged(const QColor& color)
{
    m_color = color;
    repaint();
}

void WaterMarkWidget::waterMarkVisibleChanged(bool state)
{
    setVisible(state);
}

void WaterMarkWidget::TextChanged(const QString& text)
{
    m_text = text;
    QFontMetrics fontMetrics(m_font);
    QRect rect = fontMetrics.boundingRect(QRect(0, 0, 1920, 1080), Qt::AlignLeft, m_text);
    resize(rect.size());
    repaint();
}
