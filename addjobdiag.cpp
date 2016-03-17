#include "addjobdiag.h"
#include "ui_addjobdiag.h"

#include <QSize>
#include <QString>
#include <QStringList>
#include <QTime>
#include <QDate>
#include <QColor>
#include <QFontDatabase>
#include <QDebug>

AddJobDiag::AddJobDiag(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddJobDiag)
{
    ui->setupUi(this);

    init();

    m_sizeMap["1280 × 720"] = QSize(1280, 720);
    m_sizeMap["1280 × 960"] = QSize(1280, 960);
    m_sizeMap["1920 × 1080"] = QSize(1920, 1080);
    m_sizeMap["1920 × 1200"] = QSize(1920, 1200);
    m_sizeMap["2048 × 1748"] = QSize(2048, 1748);
    m_sizeMap["2560 × 1600"] = QSize(2560, 1600);
    m_sizeMap["3840 × 2160"] = QSize(3840, 2160);
    m_sizeMap["4960 × 2790"] = QSize(4960, 2790);
    m_sizeMap["4960 × 3508"] = QSize(4960, 3508);
    m_sizeMap["7680 × 4320"] = QSize(7680, 4320);
    m_sizeMap["7680 × 4800"] = QSize(7680, 4800);

    ui->comboBox->clear();
    QStringList items;
    items.append(m_sizeMap.keys());
    ui->comboBox->addItems(items);
    ui->fontsizecombox->setCurrentText("36");
    ui->textedit->clear();
    ui->textedit->setPlainText("Sample Text");
}

AddJobDiag::~AddJobDiag()
{
    final();
    delete ui;
}

void AddJobDiag::init()
{
    SetFontSizeCombox(QFont());

//    connect(ui->fontComboBox, SIGNAL(currentFontChanged(QFont)), this, SIGNAL(fontFamilyChanged(QFont)));
    connect(ui->fontboldbtn, SIGNAL(toggled(bool)), this, SIGNAL(BoldStateChanged(bool)));
    connect(ui->fontitalicbtn, SIGNAL(toggled(bool)), this, SIGNAL(ItalyStateChanged(bool)));
    connect(ui->watermarkchkbox, SIGNAL(toggled(bool)), this, SIGNAL(WaterMarkVisibleChanged(bool)));
}

void AddJobDiag::final()
{
//    disconnect(ui->fontComboBox, SIGNAL(currentFontChanged(QFont)), this, SIGNAL(fontFamilyChanged(QFont)));
    disconnect(ui->fontboldbtn, SIGNAL(toggled(bool)), this, SIGNAL(BoldStateChanged(bool)));
    disconnect(ui->fontitalicbtn, SIGNAL(toggled(bool)), this, SIGNAL(ItalyStateChanged(bool)));
    disconnect(ui->watermarkchkbox, SIGNAL(toggled(bool)), this, SIGNAL(WaterMarkVisibleChanged(bool)));
}

QByteArray AddJobDiag::GetAddRenderTaskString()
{
    RenderData data;
    QString fileName;
    data.size = m_sizeMap[ui->comboBox->currentText()];

    if(ui->mediumBtn->isChecked()) {
        fileName.append("MEDIUM");
        data.quality = 0;
    } else if (ui->highBtn->isChecked()) {
        fileName.append("HIGH");
        data.quality = 1;
    } else if (ui->veryHighBtn->isChecked()) {
        fileName.append("VERY HIGH");
        data.quality = 2;
    } else {
        fileName.append("MEDIUM");
        data.quality = 0;
    }

    fileName.append("_");
    fileName.append(QString::number(data.size.width()));
    fileName.append("_");
    fileName.append(QString::number(data.size.height()));
    fileName.append("_");
    fileName.append(QDateTime::currentDateTime().date().toString("yyyyMMdd"));
    fileName.append("_");
    fileName.append(QDateTime::currentDateTime().time().toString("hhmmsszzz"));

    if(ui->jpgBtn->isChecked()) {
        data.format = 0;
        fileName.append(".jpg");
    } else if(ui->tifBtn->isChecked()) {
        fileName.append(".tif");
        data.format = 1;
    } else if(ui->pngBtn->isChecked()){
        fileName.append(".png");
        data.format = 2;
    } else  {
        fileName.append(".jpg");
        data.format = 0;
    }

    qDebug() << fileName;
    data.fileName = fileName;

    QByteArray str("AddRenderTask:");
    unsigned uSize = 0x00;
    uSize = data.size.width();
    uSize = uSize << 16;
    uSize |= data.size.height();
    str.append(QString("FileName:%1;").arg(data.fileName));
    str.append(QString("SIZE:%1;").arg(uSize));
    str.append(QString("FORMAT:%1;").arg(data.format));
    str.append(QString("QUALITY:%1;").arg(data.quality));

    return str;
}

void AddJobDiag::on_addTaskBtn_clicked()
{
    RenderData data;
    QString fileName;
    data.size = m_sizeMap[ui->comboBox->currentText()];

    if(ui->mediumBtn->isChecked()) {
        fileName.append("MEDIUM");
        data.quality = 0;
    } else if (ui->highBtn->isChecked()) {
        fileName.append("HIGH");
        data.quality = 1;
    } else if (ui->veryHighBtn->isChecked()) {
        fileName.append("VERY HIGH");
        data.quality = 2;
    } else {
        fileName.append("MEDIUM");
        data.quality = 0;
    }

    fileName.append("_");
    fileName.append(QString::number(data.size.width()));
    fileName.append("_");
    fileName.append(QString::number(data.size.height()));
    fileName.append("_");
    fileName.append(QDateTime::currentDateTime().date().toString("yyyyMMdd"));
    fileName.append("_");
    fileName.append(QDateTime::currentDateTime().time().toString("hhmmsszzz"));

    if(ui->jpgBtn->isChecked()) {
        data.format = 0;
        fileName.append(".jpg");
    } else if(ui->tifBtn->isChecked()) {
        fileName.append(".tif");
        data.format = 1;
    } else if(ui->pngBtn->isChecked()){
        fileName.append(".png");
        data.format = 2;
    } else  {
        fileName.append(".jpg");
        data.format = 0;
    }

    data.fileName = fileName;

    unsigned uSize = 0x00;
    uSize = data.size.width();
    uSize = uSize << 16;
    uSize |= data.size.height();

    emit AddNewTask(data);
}

void AddJobDiag::on_colorlineEdit_editingFinished()
{
    QString colorStr = ui->colorlineEdit->text();
    if(colorStr.startsWith("#")) {
        QString colorDataStr = colorStr.mid(1, 6);
        bool ok = false;
        uint ucolordata = colorDataStr.toUInt(&ok, 16);
        if(ok) {
            QColor color(ucolordata);
            ui->colorbglabel->setStyleSheet(QString("background-color: rgba(%1, %2, %3, %4);").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha()));
            emit TextColorChanged(color);
        }
    }
}

void AddJobDiag::on_fontsizecombox_currentIndexChanged(const QString &arg1)
{
    bool ok = false;
    uint size = arg1.toUInt(&ok);
    if(ok) {
        emit fontSizeChanged(size);
    }
}

void AddJobDiag::on_textedit_textChanged()
{
    emit TextChanged(ui->textedit->toPlainText());
}

void AddJobDiag::on_fontComboBox_currentFontChanged(const QFont &f)
{
    SetFontSizeCombox(f);

    ui->fontboldbtn->setChecked(false);
    ui->fontitalicbtn->setChecked(false);
    ui->fontsizecombox->setCurrentText("36");
    emit fontFamilyChanged(f);
    emit fontSizeChanged(36);
}

void AddJobDiag::SetFontSizeCombox(const QFont& f)
{
    QFontDatabase database;
    QList<int> list = database.pointSizes(f.family());
    ui->fontsizecombox->clear();
    foreach(int size, list) {
        ui->fontsizecombox->addItem(QString::number(size));
    }
}
