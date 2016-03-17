#ifndef DELETEDIALOG_H
#define DELETEDIALOG_H

#include <QDialog>
#include <QColor>

namespace Ui {
class DeleteDialog;
}

class DeleteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeleteDialog(QWidget *parent = 0);
    ~DeleteDialog();

    void SetBackgroundColor(const QColor&);

private:
    Ui::DeleteDialog *ui;

signals:
    void DeletePressed();
    void CancelPressed();

private slots:
    void on_deletepanedelBtn_clicked();
    void on_deletepanecancelBtn_clicked();
};

#endif // DELETEDIALOG_H
