#include "itemmodelex.h"
#include "ui_itemmodelex.h"

#include <QPainter>
#include <QPixmap>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QPaintEvent>

ItemModelEx::ItemModelEx(QWidget *parent, Qt::WindowFlags flags)
    :ItemBase(parent, flags)
    ,ui(new Ui::ItemModelEx)
    ,m_status(ButtonStatus_Normal)
    ,m_bResourceInit(false)
{
    ui->setupUi(this);
    ui->text->hide();
    m_pScrollLabel = new ScrollLabelEx(ui->background);
    m_pScrollLabel->setGeometry(QRect(168, 19, 102, 22));
    /* Must to set font, to make scroll text can realize the font info, styleSheet can't update the font info immedicate */
    m_pScrollLabel->SetPixmapsPerFrame(-3);
    m_pScrollLabel->setFont(QFont("Arial", 13));
    m_pScrollLabel->SetTextLabelStyleSheet(QLatin1String("font: 13pt \"Arial\";\n"
"color: rgb(255, 255, 255);"));
    OnInit();
}

ItemModelEx::ItemModelEx(const QPixmap& image, const QString& text, QWidget *parent, Qt::WindowFlags flags)
    :ItemBase(parent, flags)
    ,ui(new Ui::ItemModelEx)
    ,m_status(ButtonStatus_Normal)
    ,m_bResourceInit(false)
{
    ui->setupUi(this);
    OnInit();

    SetIconImage(image);
    SetText(text);
}

ItemModelEx::~ItemModelEx()
{
    OnFinaliaze();
}

void ItemModelEx::SetStatus(Status status)
{
    if((m_status == status) || (status == ButtonStatus_NotMePressed))
        return;

    // update status
    m_status = status;

    switch(m_status) {
    case ButtonStatus_NotMePressed:
        break;
    default:
        break;
    }
}

void ItemModelEx::OnInit()
{
    connect(ui->pushButton, SIGNAL(clicked()), this, SIGNAL(MouseButtonClicked()));
    connect(&m_timeline, SIGNAL(frameChanged(int)), this, SLOT(OnFrameChanged(int)));
}

void ItemModelEx::OnFinaliaze()
{
    disconnect(ui->pushButton, SIGNAL(clicked()), this, SIGNAL(MouseButtonClicked()));
    disconnect(&m_timeline, SIGNAL(frameChanged(int)), this, SLOT(OnFrameChanged(int)));
}

void ItemModelEx::enterEvent(QEvent *)
{
 //   ui->effectbackground->setStyleSheet(QString("border-image: url(:/configure/Resource/configure/u74.png);"));
}

void ItemModelEx::leaveEvent(QEvent *)
{
//    ui->effectbackground->setStyleSheet(QString("border-image: url(:/configure/Resource/configure/u74.png);"));
}

void ItemModelEx::SetIconImage(const QString& image)
{
    ui->icon->setPixmap(QPixmap(image));
}

void ItemModelEx::SetIconImage(const QPixmap& pixmap)
{
    ui->icon->setPixmap(pixmap);
}

void ItemModelEx::SetText(const QString& text)
{
 //   ui->text->setText(text);
    m_pScrollLabel->SetText(text);
}

void ItemModelEx::SetProgressBar(int value)
{
    ui->progressBar->setValue(value);
}

void ItemModelEx::SetProgressBarVisible(bool visible)
{
    m_graphicsOpacityEffect.setOpacity(1);
    ui->progressBar->setGraphicsEffect(&m_graphicsOpacityEffect);
    ui->progressBar->setVisible(visible);
}

void ItemModelEx::on_pushButton_clicked()
{
    emit ButtonPressed();
}

void ItemModelEx::SetButtonText(const QString& text)
{
    ui->pushButton->setText(text);
}

int ItemModelEx::GetProgressBarValue() const
{
    return ui->progressBar->value();
}

void ItemModelEx::StartProgressBarFade()
{
    m_timeline.setDuration(3000);
    m_timeline.setFrameRange(0, 100);
    m_timeline.start();
}

void ItemModelEx::OnFrameChanged(int index)
{
    float opacity = 1 - index * 1.0f/ m_timeline.endFrame();
    m_graphicsOpacityEffect.setOpacity(opacity);
    ui->progressBar->setGraphicsEffect(&m_graphicsOpacityEffect);
}

void ItemModelEx::SetDownloadWidgetVisible(bool visible)
{
    ui->widget->setVisible(visible);
}

void ItemModelEx::SetDownloadStateWidgetText(const QString& text)
{
    ui->statetext->setText(text);
}
