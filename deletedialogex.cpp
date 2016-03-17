#include "deletedialogex.h"
#include "ui_deletedialogex.h"

DeleteDialogEx::DeleteDialogEx(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeleteDialogEx)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);

    ui->setupUi(this);
}

DeleteDialogEx::~DeleteDialogEx()
{
    delete ui;
}

void DeleteDialogEx::on_deletepanedelBtn_clicked()
{
    accept();
}

void DeleteDialogEx::on_deletepanecancelBtn_clicked()
{
    reject();
}

void DeleteDialogEx::SetBackgroundColor(const QColor& color)
{
    QString stylesheet = QStringLiteral("background-color: rgba(%1, %2, %3, %4);").arg(QString::number(color.red()))
                                                                .arg(QString::number(color.green()))
                                                                .arg(QString::number(color.blue()))
                                                                .arg(QString::number(color.alpha())) ;

//    qDebug() << stylesheet;
    ui->background->setStyleSheet( stylesheet );
    setStyleSheet(stylesheet);
}
