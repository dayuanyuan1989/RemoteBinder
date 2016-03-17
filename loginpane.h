#ifndef LOGINPANE_H
#define LOGINPANE_H

#include <QWidget>
#include <QList>
#include <QString>
#include <QStringList>

#include "uianimationmanager.h"

namespace Ui {
class LoginPane;
}

class LoginPane : public QWidget
{
    Q_OBJECT

    enum SetId { SetId_Invalid, SetId_Add, SetId_Modify, SetId_Delete, SetId_Count };

public:
    explicit LoginPane(QWidget *parent = 0);
    ~LoginPane();

    virtual bool eventFilter(QObject *, QEvent *);

private:
    void init();
    void final();

private:
    Ui::LoginPane *ui;

    SetId               m_curSetId;
    UIAnimationManager  m_animangager;

signals:
    void QuitRequest();
    void PageJump(int);
    void ConnectRequest(const QString& address, uint port);
    void ServerListUpdate(const QStringList&);

private slots:
    void on_addbtn_clicked();
    void on_editbtn_clicked();
    void on_deletebtn_clicked();
    void on_setsubmitbtn_clicked();
    void on_setcancelbtn_clicked();
    void on_submitbtn_clicked();
    void on_quitbtn_clicked();
    void on_ipcombox_activated(int index);

    void OnAnimaitionFinished();
};

#endif // LOGINPANE_H
