#include "covercotrolpane.h"
#include "ui_covercotrolpane.h"

CoverCotrolPane::CoverCotrolPane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CoverCotrolPane)
{
    ui->setupUi(this);

    init();
}

CoverCotrolPane::~CoverCotrolPane()
{
    final();
    delete ui;
}

void CoverCotrolPane::init()
{
    m_pModeGrp = new QButtonGroup(this);
    m_pModeGrp->addButton(ui->previewbtn, 0);
    m_pModeGrp->addButton(ui->plorebtn, 1);
    m_pModeGrp->addButton(ui->configurebtn, 2);

    connect(m_pModeGrp, SIGNAL(buttonToggled(int,bool)), this, SLOT(ModeButtonGrpToggle(int, bool)));

    m_pSetHelpDiag = new SettingHelpDiag(this);
    m_pSetHelpDiag->raise();
    m_pSetHelpDiag->move(1505, 110);
    m_pSetHelpDiag->hide();

    m_pAddJobDiag = new AddJobDiag(this);
    m_pAddJobDiag->raise();
    m_pAddJobDiag->move(5, 112);
    m_pAddJobDiag->hide();
}

void CoverCotrolPane::final()
{
    disconnect(m_pModeGrp, SIGNAL(buttonToggled(int,bool)), this, SLOT(ModeButtonGrpToggle(int, bool)));
}

void CoverCotrolPane::SetParentOnBackground(QWidget* pwidget)
{
    pwidget->setParent(ui->background);
}

void CoverCotrolPane::ModeButtonGrpToggle(int index, bool checked)
{
    if(checked) {
        emit ModeChanged(index);
    }
}

void CoverCotrolPane::on_setbtn_toggled(bool checked)
{
    if(checked) {
        m_pSetHelpDiag->setVisible(true);
        if(ui->helpbtn->isChecked()) ui->helpbtn->setChecked(false);
        m_pSetHelpDiag->SetPaneVisible(true);
        m_pSetHelpDiag->HelpPaneVisible(false);
    } else {
        if(!ui->helpbtn->isChecked()) m_pSetHelpDiag->setVisible(false);
    }
}

void CoverCotrolPane::on_helpbtn_toggled(bool checked)
{
    if(checked) {
        m_pSetHelpDiag->setVisible(true);
        if(ui->setbtn->isChecked()) ui->setbtn->setChecked(false);
        m_pSetHelpDiag->SetPaneVisible(false);
        m_pSetHelpDiag->HelpPaneVisible(true);
    } else {
        if(!ui->setbtn->isChecked()) m_pSetHelpDiag->setVisible(false);
    }
}

void CoverCotrolPane::on_connectbtn_clicked()
{
    emit RequesetJumpPage(0);
}

void CoverCotrolPane::on_addbtn_toggled(bool checked)
{
    if(checked) {
        m_pAddJobDiag->show();
        if(ui->listbtn->isChecked())    ui->listbtn->setChecked(false);
    } else {
        m_pAddJobDiag->hide();
    }
}

void CoverCotrolPane::on_listbtn_toggled(bool checked)
{
    if(checked) {
        if(ui->addbtn->isChecked()) ui->addbtn->setChecked(false);
    } else{
//         if(!ui->addbtn->isChecked())
    }
}
