#ifndef COVERCOTROLPANE_H
#define COVERCOTROLPANE_H

#include <QWidget>
#include <QButtonGroup>

#include "settinghelpdiag.h"
#include "addjobdiag.h"

namespace Ui {
class CoverCotrolPane;
}

class CoverCotrolPane : public QWidget
{
    Q_OBJECT

public:
    explicit CoverCotrolPane(QWidget *parent = 0);
    ~CoverCotrolPane();

    void SetParentOnBackground(QWidget* pwidget);

private:
    void init();
    void final();

private:
    Ui::CoverCotrolPane *ui;
    QButtonGroup*       m_pModeGrp;
    SettingHelpDiag*    m_pSetHelpDiag;
    AddJobDiag*         m_pAddJobDiag;

signals:
    void ModeChanged(int);
    void RequesetJumpPage(int);

private slots:
    void ModeButtonGrpToggle(int, bool);
    void on_setbtn_toggled(bool checked);
    void on_helpbtn_toggled(bool checked);
    void on_connectbtn_clicked();
    void on_addbtn_toggled(bool checked);
    void on_listbtn_toggled(bool checked);
};

#endif // COVERCOTROLPANE_H
