#include "settinghelpdiag.h"
#include "ui_settinghelpdiag.h"

SettingHelpDiag::SettingHelpDiag(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingHelpDiag)
{
    ui->setupUi(this);

    ui->servercombox->clear();

    ui->offgrp->installEventFilter(this);
    ui->offgrp_2->installEventFilter(this);
    ui->ongrp->installEventFilter(this);
    ui->ongrp_2->installEventFilter(this);

    ui->ongrp->show();
    ui->offgrp->hide();
    ui->ongrp_2->show();
    ui->offgrp_2->hide();
}

SettingHelpDiag::~SettingHelpDiag()
{
    ui->offgrp->removeEventFilter(this);
    ui->offgrp_2->removeEventFilter(this);
    ui->ongrp->removeEventFilter(this);
    ui->ongrp_2->removeEventFilter(this);

    delete ui;
}

void SettingHelpDiag::SetPaneVisible(bool visible)
{
    ui->setgrp->setVisible(visible);
}

void SettingHelpDiag::HelpPaneVisible(bool visible)
{
    ui->helpgrp->setVisible(visible);
}

bool SettingHelpDiag::eventFilter(QObject *obj, QEvent *e)
{
    if(e->type() == QEvent::MouseButtonRelease) {
        if((obj == ui->offgrp)) {
            ui->offgrp->hide();
            ui->ongrp->show();
            emit CheckBoxChanged(0, true);
        } else if(obj == ui->ongrp){
            ui->offgrp->show();
            ui->ongrp->hide();
            emit CheckBoxChanged(0, false);
        } else if(obj == ui->offgrp_2) {
            ui->offgrp_2->hide();
            ui->ongrp_2->show();
            emit CheckBoxChanged(1, true);
        } else if(obj == ui->ongrp_2) {
            ui->offgrp_2->show();
            ui->ongrp_2->hide();
            emit CheckBoxChanged(1, false);
        } else {

        }
    }

    return obj->event(e);
}

void SettingHelpDiag::on_applybtn_clicked()
{
    emit ServerReConnect(ui->servercombox->currentIndex());
}

void SettingHelpDiag::UpdateServerList(const QStringList& strlist)
{
    ui->servercombox->clear();
    ui->servercombox->addItems(strlist);
}
