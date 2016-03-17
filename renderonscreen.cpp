#include "renderonscreen.h"
#include "ui_renderonscreen.h"
#include <QMovie>

RenderOnScreen::RenderOnScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RenderOnScreen)
{
    ui->setupUi(this);

    QMovie* pMoive = new QMovie(":/configure/Resource/configure/IMG_0755.GIF");
    ui->moivelabel->setMovie(pMoive);
    pMoive->start();
}

RenderOnScreen::~RenderOnScreen()
{
    delete ui;
}
