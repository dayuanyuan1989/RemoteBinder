#ifndef SETTINGHELPDIAG_H
#define SETTINGHELPDIAG_H

#include <QWidget>
#include <QStringList>

class CoverCotrolPane;

namespace Ui {
class SettingHelpDiag;
}

class SettingHelpDiag : public QWidget
{
    Q_OBJECT

public:
    explicit SettingHelpDiag(QWidget *parent = 0);
    ~SettingHelpDiag();

    void SetPaneVisible(bool);
    void HelpPaneVisible(bool);

    void UpdateServerList(const QStringList&);

    virtual bool eventFilter(QObject *, QEvent *);

private:
    Ui::SettingHelpDiag *ui;

    friend class CoverCotrolPane;

signals:
    void CheckBoxChanged(int, bool);
    void ServerReConnect(int index);

private slots:
    void on_applybtn_clicked();
};

#endif // SETTINGHELPDIAG_H
