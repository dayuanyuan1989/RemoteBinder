#include "jobtaskitem.h"
#include "ui_jobtaskitem.h"

JobTaskItem::JobTaskItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JobTaskItem)
{
    ui->setupUi(this);

//    SetBodyVisible(false);
}

JobTaskItem::~JobTaskItem()
{
    delete ui;
}

void JobTaskItem::SetTitle(const QString& str)
{
    ui->title->setText(str);
}

void JobTaskItem::SetSizeValue(const QString& str)
{
    ui->size->setText(str);
}

void JobTaskItem::SetQualityValue(const QString& str)
{
    ui->quality->setText(str);
}

void JobTaskItem::SetBodyVisible(bool visible)
{
    if(visible) {
        resize(width(), JOB_ITEM_TOTAL_HEIGHT);
    } else {
        resize(width(), JOB_ITEM_HEAD_HEIGHT);
    }
}

bool JobTaskItem::BodyVisible()
{
    return (height() != JOB_ITEM_HEAD_HEIGHT);
}
