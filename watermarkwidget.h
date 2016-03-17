#ifndef WATERMARKWIDGET_H
#define WATERMARKWIDGET_H

#include <QWidget>

class QPaintEvent;
class QMouseEvent;
class QFont;
class QColor;
class QString;
class QPoint;

namespace Ui {
class WaterMarkWidget;
}

class WaterMarkWidget : public QWidget
{
    Q_OBJECT

public:
    struct WaterMarkInfo
    {
    public:
        WaterMarkInfo() {}
        WaterMarkInfo(bool lvisible,
                      const QFont& lfont,
                      const QColor& lcolor,
                      const QString& ltext,
                      const QPoint& lpoint)
        {
            visible = lvisible;
            font = lfont;
            color = lcolor;
            text = ltext;
            pos = lpoint;
        }

        bool    visible;
        QFont   font;
        QColor  color;
        QPoint  pos;
        QString text;
    };

    explicit WaterMarkWidget(QWidget *parent = 0);
    ~WaterMarkWidget();

    WaterMarkInfo GetWaterMarkInfo() const
    {
        return WaterMarkInfo(isVisible(), m_font, m_color, m_text, pos());
    }

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);

public slots:
    void waterMarkVisibleChanged(bool);
    void fontFamilyChanged(const QFont&);
    void fontSizeChanged(uint);
    void BoldStateChanged(bool);
    void ItalyStateChanged(bool);
    void TextColorChanged(const QColor&);
    void TextChanged(const QString&);

private:
    Ui::WaterMarkWidget *ui;

    QFont               m_font;
    QColor              m_color;
    QString             m_text;

    QPoint              m_lastPoint;
    QPoint              m_lastGloablPoint;
};

#endif // WATERMARKWIDGET_H
