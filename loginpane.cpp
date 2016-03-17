#include "loginpane.h"
#include "ui_loginpane.h"
#include "deletedialog.h"
#include "resourcemanager.h"
#include "logmanager.h"

#include <QEvent>

LoginPane::LoginPane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginPane),
    m_curSetId(SetId_Invalid)
{
    ui->setupUi(this);

    init();
}

LoginPane::~LoginPane()
{
    final();

    delete ui;
}

void LoginPane::init()
{
    ui->setpane->hide();
    ui->deletepane->hide();

    ui->ipcombox->clear();

    QStringList titleList;
    foreach(LoginInfo loginInfo, LoginInfoList) {
        titleList.append(loginInfo.title);
    }
    ui->ipcombox->addItems(titleList);

    ui->serveredit->installEventFilter(this);
    ui->portedit->installEventFilter(this);
    ui->labeledit->installEventFilter(this);

    connect(&m_animangager, SIGNAL(finished()), this, SLOT(OnAnimaitionFinished()));
}

void LoginPane::final()
{
    ui->serveredit->removeEventFilter(this);
    ui->portedit->removeEventFilter(this);
    ui->labeledit->removeEventFilter(this);

    disconnect(&m_animangager, SIGNAL(finished()), this, SLOT(OnAnimaitionFinished()));
}

void LoginPane::on_addbtn_clicked()
{
    m_curSetId = SetId_Add;

    ui->setpane->setVisible(true);
    ui->serveredit->clear();
    ui->portedit->clear();
    ui->labeledit->clear();

    ui->portedit->setText("4500");

    QRect startValue(1250, 411, 170, 30);
    QRect endValue(357, 373, 1130, 397);
    m_animangager.AddAnimation(ui->setpane, UIAnimationManager::ActionType_Geometry, startValue, endValue);
    m_animangager.SetDuration(300);
    m_animangager.StartAnimation();
}

void LoginPane::on_editbtn_clicked()
{
    m_curSetId = SetId_Modify;

    int curComboIndex = ui->ipcombox->currentIndex();
    if( -1 == curComboIndex )
        return;

    ui->serveredit->setText(LoginInfoList[curComboIndex].server);
    ui->portedit->setText(QString::number(LoginInfoList[curComboIndex].port));
    ui->labeledit->setText(LoginInfoList[curComboIndex].title);

    ui->setpane->setVisible(true);

    QRect startValue(1414, 411, 170, 30);
    QRect endValue(357, 373, 1130, 397);
    m_animangager.AddAnimation(ui->setpane, UIAnimationManager::ActionType_Geometry, startValue, endValue);
    m_animangager.SetDuration(300);
    m_animangager.StartAnimation();
}

void LoginPane::on_deletebtn_clicked()
{
    m_curSetId = SetId_Delete;

    int curComboIndex = ui->ipcombox->currentIndex();
    if( -1 == curComboIndex )
        return;

    DeleteDialog diag(this);
    diag.move(ui->deletepane->mapToGlobal(QPoint(0, 0)));
    if(QDialog::Accepted == diag.exec()) {
        ui->serveredit->clear();
        ui->portedit->clear();
        ui->labeledit->clear();
        ui->setpane->hide();

        int curComboIndex = ui->ipcombox->currentIndex();
        if( -1 != curComboIndex ) {
            LoginInfoList.takeAt(curComboIndex);
            ui->ipcombox->removeItem(curComboIndex);
        }
    }
}

void LoginPane::on_setsubmitbtn_clicked()
{
    LoginInfo info;
    info.title = ui->labeledit->text();
    info.port = ui->portedit->text().toUInt();
    info.server = ui->serveredit->text();

    int curComboIndex = ui->ipcombox->currentIndex();
    if(SetId_Add == m_curSetId) {
        curComboIndex++;
        LoginInfoList.insert(curComboIndex, info);
        ui->ipcombox->insertItem(curComboIndex, info.title);
        ui->ipcombox->setCurrentIndex(curComboIndex);
    } else if(SetId_Modify == m_curSetId) {
        if(LoginInfoList.count() > 0)
            LoginInfoList.replace(curComboIndex, info);
            ui->ipcombox->setItemText(curComboIndex, info.title);
    } else{

    }

    QRect startValue(357, 373, 1130, 397);
    QRect endValue(220, 253, 1000, 100);
    m_animangager.AddAnimation(ui->setpane, UIAnimationManager::ActionType_Geometry, startValue, endValue);
    m_animangager.SetDuration(300);
    m_animangager.StartAnimation();

    m_curSetId = SetId_Invalid;
}

void LoginPane::on_setcancelbtn_clicked()
{
    ui->setpane->setVisible(false);
}

void LoginPane::on_submitbtn_clicked()
{
    emit PageJump(1);

    int curComboIndex = ui->ipcombox->currentIndex();
    if( ( -1 == curComboIndex ) && (LoginInfoList.count()) == 0)
        return;

    emit ConnectRequest(LoginInfoList[curComboIndex].server, LoginInfoList[curComboIndex].port);

    QStringList list;
    foreach(LoginInfo info, LoginInfoList) {
        list.append(info.title);
    }
    emit ServerListUpdate(list);
}

void LoginPane::on_quitbtn_clicked()
{
    emit QuitRequest();
}

void LoginPane::on_ipcombox_activated(int index)
{
    if(ui->setpane->isVisible()) {
        ui->serveredit->setText(LoginInfoList[index].server);
        ui->portedit->setText(QString::number(LoginInfoList[index].port));
        ui->labeledit->setText(LoginInfoList[index].title);
    }
}

void LoginPane::OnAnimaitionFinished()
{
 //   ui->setpane->hide();
}

bool LoginPane::eventFilter(QObject *obj, QEvent *e)
{
    if(obj == ui->serveredit || obj == ui->portedit || obj == ui->labeledit) {
        //LogManagerInst << LogManager::LogDegreeType_Warn << QString::number(e->type());
        if(e->type() == QEvent::MouseButtonPress) {
            CommonUtils::ShowTabTip();
        }
    }

    return obj->event(e);
}
