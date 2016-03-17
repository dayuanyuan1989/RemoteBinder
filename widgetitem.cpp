#include "widgetitem.h"
#include "ui_widgetitem.h"
#include <QResizeEvent>

WidgetItem::WidgetItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetItem)
{
    ui->setupUi(this);
}

WidgetItem::~WidgetItem()
{
    delete ui;
}

void WidgetItem::SetIcon(const QImage& image)
{
    ui->icon->setPixmap(QPixmap::fromImage(image));
    ui->icon->setScaledContents(true);
}

void WidgetItem::SetText(const QString& text)
{
    ui->text->setText(text);
}

void WidgetItem::SetBackgroundColor(const QColor& color)
{
    QString style = QString("background-color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue());
    ui->background->setStyleSheet(style);
}

void WidgetItem::resizeEvent(QResizeEvent *e)
{
    QSize newSize = e->size();
    QRect rect = ui->text->geometry();
    rect.setBottomRight(QPoint(newSize.width()-50, rect.bottom()));
    ui->text->setGeometry(rect);
    ui->background->resize(newSize);
}
