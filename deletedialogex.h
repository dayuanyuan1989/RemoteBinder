#ifndef DELETEDIALOGEX_H
#define DELETEDIALOGEX_H

#include <QDialog>

namespace Ui {
class DeleteDialogEx;
}

class DeleteDialogEx : public QDialog
{
    Q_OBJECT

public:
    explicit DeleteDialogEx(QWidget *parent = 0);
    ~DeleteDialogEx();

    void SetBackgroundColor(const QColor&);

private:
    Ui::DeleteDialogEx *ui;

signals:
    void DeletePressed();
    void CancelPressed();

private slots:
    void on_deletepanedelBtn_clicked();
    void on_deletepanecancelBtn_clicked();
};

#endif // DELETEDIALOGEX_H
