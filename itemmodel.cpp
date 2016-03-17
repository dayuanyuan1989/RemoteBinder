#include "itemmodel.h"
#include "ui_itemmodel.h"

#include <QPainter>
#include <QPixmap>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QPaintEvent>

ItemModel::ItemModel(QWidget *parent, Qt::WindowFlags flags)
    :ItemBase(parent, flags)
    ,ui(new Ui::ItemModel)
    ,m_status(ButtonStatus_Normal)
    ,m_bResourceInit(false)
{
    ui->setupUi(this);
	OnInit();
}

ItemModel::ItemModel(const QPixmap& image, const QString& text, QWidget *parent, Qt::WindowFlags flags)
    :ItemBase(parent, flags)
    ,ui(new Ui::ItemModel)
    ,m_status(ButtonStatus_Normal)
    ,m_bResourceInit(false)
{
    ui->setupUi(this);
    OnInit();

    SetIconImage(image);
    SetText(text);
}

ItemModel::~ItemModel()
{
	OnFinaliaze();
}

void ItemModel::SetStatus(Status status)
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

void ItemModel::OnInit()
{
    connect(ui->pushButton, SIGNAL(clicked()), this, SIGNAL(MouseButtonClicked()));
    connect(&m_timeline, SIGNAL(frameChanged(int)), this, SLOT(OnFrameChanged(int)));
}

void ItemModel::OnFinaliaze()
{
    disconnect(ui->pushButton, SIGNAL(clicked()), this, SIGNAL(MouseButtonClicked()));
    disconnect(&m_timeline, SIGNAL(frameChanged(int)), this, SLOT(OnFrameChanged(int)));
}

void ItemModel::enterEvent(QEvent *)
{
 //   ui->effectbackground->setStyleSheet(QString("border-image: url(:/configure/Resource/configure/u74.png);"));
}

void ItemModel::leaveEvent(QEvent *)
{
//    ui->effectbackground->setStyleSheet(QString("border-image: url(:/configure/Resource/configure/u74.png);"));
}

void ItemModel::SetIconImage(const QString& image)
{
    ui->icon->setPixmap(QPixmap(image));
}

void ItemModel::SetIconImage(const QPixmap& pixmap)
{
    ui->icon->setPixmap(pixmap);
}

void ItemModel::SetText(const QString& text)
{
    ui->text->setText(text);
}

void ItemModel::SetProgressBar(int value)
{
    ui->progressBar->setValue(value);
}

void ItemModel::SetProgressBarVisible(bool visible)
{
    m_graphicsOpacityEffect.setOpacity(1);
    ui->progressBar->setGraphicsEffect(&m_graphicsOpacityEffect);
    ui->progressBar->setVisible(visible);
}

void ItemModel::on_pushButton_clicked()
{
    emit ButtonPressed();
}

void ItemModel::SetButtonText(const QString& text)
{
    ui->pushButton->setText(text);
}

int ItemModel::GetProgressBarValue() const
{
    return ui->progressBar->value();
}

void ItemModel::StartProgressBarFade()
{
    m_timeline.setDuration(3000);
    m_timeline.setFrameRange(0, 100);
    m_timeline.start();
}

void ItemModel::OnFrameChanged(int index)
{
    float opacity = 1 - index * 1.0f/ m_timeline.endFrame();
    m_graphicsOpacityEffect.setOpacity(opacity);
    ui->progressBar->setGraphicsEffect(&m_graphicsOpacityEffect);
}
