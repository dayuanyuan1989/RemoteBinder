#include "deletedialog.h"
#include "ui_deletedialog.h"


DeleteDialog::DeleteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeleteDialog)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);

    ui->setupUi(this);
}

DeleteDialog::~DeleteDialog()
{
    delete ui;
}

void DeleteDialog::on_deletepanedelBtn_clicked()
{
    accept();
}

void DeleteDialog::on_deletepanecancelBtn_clicked()
{
    reject();
}

void DeleteDialog::SetBackgroundColor(const QColor& color)
{
    QString stylesheet = QStringLiteral("background-color: rgba(%1, %2, %3, %4);").arg(QString::number(color.red()))
                                                                .arg(QString::number(color.green()))
                                                                .arg(QString::number(color.blue()))
                                                                .arg(QString::number(color.alpha())) ;

//    qDebug() << stylesheet;
    ui->background->setStyleSheet( stylesheet );
    setStyleSheet(stylesheet);
}
