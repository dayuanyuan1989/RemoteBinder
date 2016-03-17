#ifndef WIDGETITEM_H
#define WIDGETITEM_H

#include <QWidget>
#include <QImage>

namespace Ui {
class WidgetItem;
}

class ListWidget;

class WidgetItem : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetItem(QWidget *parent = 0);
    ~WidgetItem();

    void SetIcon(const QImage&);
    void SetText(const QString&);
    void SetBackgroundColor(const QColor&);

protected:
    virtual void resizeEvent(QResizeEvent *);

private:


private:
    Ui::WidgetItem *ui;

    friend class ListWidget;
};

#endif // WIDGETITEM_H
