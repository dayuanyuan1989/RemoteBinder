#include "modelselection.h"
#include "ui_modelselection.h"
#include "deletedialog.h"
#include "logmanager.h"
#include "itemmodel.h"
#include "resourcemanager.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonParseError>

#include <QDebug>

ModelSelection::ModelSelection(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ModelSelection)
    , m_currentConnectState(DeltaXBinder::ConnectState_Invalid)
    , m_curRequesetThumbnailIndex(-1)
    , m_curThumbnailSize(0)
    , m_curSetId(SetId_Invalid)
    , m_curInfoIndex(-1)
    , m_testing(false)
{
    ui->setupUi(this);
    ui->comboBox->clear();
    init();
}

ModelSelection::~ModelSelection()
{
    final();

    delete ui;
}

void ModelSelection::init()
{
    // use deleteDialogEx instead of deletepane
    ui->deletepane->hide();
    ui->setpane->hide();

    QStringList titleList;
    foreach(PbInfo pbInfo, PbInfoList) {
        titleList.append(pbInfo.title);
    }
    ui->comboBox->addItems(titleList);

    m_pModelSelectionPane = new ModelSelectionPaneEx(ui->modelpane);
    connect( m_pModelSelectionPane, SIGNAL(ButtonPressed(int)), this, SLOT(OnModelSelectionPaneItemPressed(int)) );
//    m_pModelSelectionPane = new ModelSelectionPaneEx();

    connect(&m_pbBinder, SIGNAL(BinderStatusChanged(DeltaXBinder::ConnectState)), this, SLOT(OnConnectStatusChanged(DeltaXBinder::ConnectState)));
    connect(&m_pbBinder, SIGNAL(DataReceived(QByteArray)), this, SLOT(OnDataReceived(QByteArray)));
}

void ModelSelection::final()
{
    disconnect(&m_pbBinder, SIGNAL(BinderStatusChanged(DeltaXBinder::ConnectState)), this, SLOT(OnConnectStatusChanged(DeltaXBinder::ConnectState)));
    disconnect(&m_pbBinder, SIGNAL(DataReceived(QByteArray)), this, SLOT(OnDataReceived(QByteArray)));

    if(m_pModelSelectionPane) {
        disconnect( m_pModelSelectionPane, SIGNAL(ButtonPressed(int)), this, SLOT(OnModelSelectionPaneItemPressed(int)) );
    }
}

void ModelSelection::on_refreshBtn_clicked()
{
    m_curSetId = SetId_Update;

    CurrentInfoConnect(m_curInfoIndex);
}

void ModelSelection::on_addBtn_clicked()
{
    m_curSetId = SetId_Add;
    ui->setpane->setVisible(true);

    {
        SetTestButtonBackOff();
        ui->titleedit->clear();
        ui->serveredit->clear();
        ui->portedit->clear();
        ui->useredit->clear();
        ui->passwordedit->clear();

        ui->portedit->setText("11000");
    }
}

void ModelSelection::on_editbtn_clicked()
{
    m_curSetId = SetId_Modify;

    if(m_curInfoIndex == -1)
        return;

    ui->setpane->setVisible(!ui->setpane->isVisible());

    if( ui->setpane->isVisible() ) {
       if( (m_curInfoIndex >= 0) && (m_curInfoIndex < PbInfoList.size()) ) {
           PbInfo info = PbInfoList[m_curInfoIndex];
           ui->titleedit->setText(info.title);
           ui->serveredit->setText(info.address);
           ui->portedit->setText(QString::number(info.port));
           ui->useredit->setText(info.user);
           ui->passwordedit->setText(info.password);
       } else {
           ui->titleedit->clear();
           ui->serveredit->clear();
           ui->portedit->clear();
           ui->useredit->clear();
           ui->passwordedit->clear();
       }
    }
}

void ModelSelection::on_deleteBtn_clicked()
{
    m_curSetId = SetId_Delete;

    if(m_curInfoIndex == -1)
        return;

    DeleteDialog diag(this);
    diag.move(ui->deletepane->mapToGlobal(QPoint(0, 0)));
    if(QDialog::Accepted == diag.exec()) {
        ui->titleedit->clear();
        ui->serveredit->clear();
        ui->portedit->clear();
        ui->useredit->clear();
        ui->passwordedit->clear();

        if( (m_curInfoIndex >= 0) && (m_curInfoIndex < PbInfoList.size()) ) {
            PbInfoList.takeAt(m_curInfoIndex);
            ui->comboBox->removeItem(m_curInfoIndex);

            if(m_curInfoIndex >= PbInfoList.count()) {
                m_curInfoIndex--;
            }
        }

        ui->setpane->hide();

        m_pModelSelectionPane->RemoveElements();
        m_pModelSelectionPane->update();
    }
}

void ModelSelection::on_testlogin_clicked()
{
    m_testing = true;
    ui->testlogin->setIcon(QIcon());
    ui->testlogin->setText("Testing...");
    ui->testlogin->setStyleSheet(
                "QPushButton {\n"
                "    border: 1px solid #8f8f91;\n"
                "	background-color: rgb(125, 154, 172);\n"
                "    min-width: 80px;\n"
                "	\n"
                "	font: 10pt \"MS Shell Dlg 2\";\n"
                "}\n"
                "\n"
                "QPushButton:pressed {\n"
                "	border: 1px solid #8f8f91;\n"
                "	background-color: rgb(117, 151, 172);\n"
                "}"
                                );

    HostInfo info;
    info.autoBinder = false;
    info.ipAddress = ui->serveredit->text();
    info.port = ui->portedit->text().toInt();
    Connect(info);
}

void ModelSelection::on_submitBtn_clicked()
{
    PbInfo info;
    info.title = ui->titleedit->text();
    info.address = ui->serveredit->text();
    info.port = ui->portedit->text().toUInt();
    info.user = ui->useredit->text();
    info.password = ui->passwordedit->text();

    if(SetId_Add == m_curSetId) {
        m_curInfoIndex++;
        PbInfoList.insert(m_curInfoIndex, info);
        ui->comboBox->insertItem(m_curInfoIndex, info.title);
        ui->comboBox->setCurrentIndex(m_curInfoIndex);
    } else if(SetId_Modify == m_curSetId) {
        if(PbInfoList.count() > 0)
            PbInfoList.replace(m_curInfoIndex, info);
            ui->comboBox->setItemText(m_curInfoIndex, info.title);
    } else{

    }

    CurrentInfoConnect(m_curInfoIndex);

    ui->setpane->hide();

    m_curSetId = SetId_Invalid;
}

void ModelSelection::on_cancelBtn_clicked()
{
    m_curSetId = SetId_Invalid;
    ui->setpane->hide();
}

void ModelSelection::on_comboBox_activated(int index)
{
//    CurrentInfoConnect(index);

    if(index >= PbInfoList.size() || index < 0)  {
        return;
    }

    m_curInfoIndex = index;

    if(ui->setpane->isVisible()) {
        PbInfo info = PbInfoList[m_curInfoIndex];
        SetTestButtonBackOff();
        ui->titleedit->setText(info.title);
        ui->serveredit->setText(info.address);
        ui->portedit->setText(QString::number(info.port));
        ui->useredit->setText(info.user);
        ui->passwordedit->setText(info.password);
    }
}

void ModelSelection::CurrentInfoConnect(int index)
{
    if(index >= PbInfoList.size() || index < 0)  {
        return;
    }

    m_curInfoIndex = index;

    if(ui->setpane->isVisible()) {
        PbInfo info = PbInfoList[m_curInfoIndex];
        SetTestButtonBackOff();
        ui->titleedit->setText(info.title);
        ui->serveredit->setText(info.address);
        ui->portedit->setText(QString::number(info.port));
        ui->useredit->setText(info.user);
        ui->passwordedit->setText(info.password);
    }

    HostInfo info;
    info.autoBinder = false;
    info.ipAddress = PbInfoList[m_curInfoIndex].address;
    info.port = PbInfoList[m_curInfoIndex].port;

    Connect(info);
}

void ModelSelection::OnConnectStatusChanged(DeltaXBinder::ConnectState state)
{
    m_currentConnectState = state;

    if(m_testing) {
        m_testing = false;
        if(DeltaXBinder::ConnectState_Connect == m_currentConnectState) {
            ui->testlogin->setIcon(QIcon(":/configure/Resource/configure/u394.png"));
            ui->testlogin->setText("Successed");
            ui->testlogin->setStyleSheet("border:0px;"
                                         "	font: 10pt \"MS Shell Dlg 2\";\n");
        } else {
            ui->testlogin->setIcon(QIcon(":/configure/Resource/configure/u392.png"));
            ui->testlogin->setText("Failed");
            ui->testlogin->setStyleSheet("border:0px;"
                                         "	font: 10pt \"MS Shell Dlg 2\";\n");
        }

        return;
    }

    if(DeltaXBinder::ConnectState_Connect != m_currentConnectState)
        return;


    m_pbBinder.sendToServiceMessage(GetCurrentLoginInfoData());

#if 0
    // request list info
    QByteArray requestData("Request Model List");
    m_pbBinder.sendToServiceMessage(requestData);
#endif
}

void ModelSelection::OnDataReceived(const QByteArray& data)
{
 //   LogManagerInst << LogManager::LogDegreeType_Normal << QByteArray::number(data.size()) << data.left(100);

    if(data.contains("Welcome"))
        return;

    QJsonParseError error;
    QJsonDocument jdocument = QJsonDocument::fromJson(data, &error);
    if (error.error == QJsonParseError::NoError) {
        if(jdocument.isObject()) {
            QJsonObject jobj = jdocument.object();
            if(jobj.keys().contains("Model List")) {
                m_modelList.clear();
                QJsonValue jvalue = jobj.value("Model List");
                if(jvalue.isArray()) {
                    QJsonArray jarray = jvalue.toArray();
                    int i = 0;
                    for(i = 0; i < jarray.count(); i++) {
                        QJsonValue jmodelVar = jarray.at(i);
                        if(jmodelVar.isObject()) {
                            QJsonObject jmodelobj =  jmodelVar.toObject();
                            ModelInfo info;
                            info.download = DownloadState_Invalid;
                            if(jmodelobj.keys().contains("Id")) {
                                info.id = jmodelobj.value("Id").toString();
                            }
                            if(jmodelobj.keys().contains("Name")) {
                                info.name = jmodelobj.value("Name").toString();
                            }
                            if(jmodelobj.keys().contains("ModelName"))  {
                                info.modelname = jmodelobj.value("ModelName").toString();
                            }
                            if(jmodelobj.keys().contains("Thumbnail Size")) {
                                info.thumbnailSize = jmodelobj.value("Thumbnail Size").toInt();
                            }
                            m_modelList.append(info);
                        }
                    }
                }
                // requeset first Thumbnial
                {
                    m_curRequesetThumbnailIndex = 0;
                    RequesetThumbnail();
                }
            }

            if(jobj.keys().contains("Download Reply")) {

                qDebug() << "Download Reply";
                QJsonValue jvalue = jobj.value("Download Reply");
                if(jvalue.isObject()) {
                    QJsonObject jdownloadreplyobj = jvalue.toObject();
                    QString id = jdownloadreplyobj.value("Id").toString();
                    bool downloadResult = jdownloadreplyobj.value("To Download").toBool();
                    bool infact = jdownloadreplyobj.value("Infact").toBool();

                    if( !downloadResult ) {
                        int index = GetIndexById(id);
                        if((-1 != index) && infact) {
                            OnDownloadFinished(index);
                        }
                    }
#if 0
                    // update download list
                    if(jdownloadreplyobj.keys().contains("Download List")) {
                        QJsonValue jvalue = jdownloadreplyobj.value("Download List");
                        if(jvalue.isArray()) {
                            m_downloadIdList.clear();
                            QJsonArray jarray = jvalue.toArray();
                            int i = 0;
                            for(i = 0; i < jarray.count(); i++) {
                                QJsonValue jmodelVar = jarray.at(i);
                                if(jmodelVar.isObject()) {
                                    QJsonObject jmodelobj =  jmodelVar.toObject();
                                    QString id;
                                    if(jmodelobj.keys().contains("id")) {
                                        id = jmodelobj.value("id").toString();
                                    }
                                    m_downloadIdList.append(id);
                                }
                            }
                        }

                    }
#endif
                }
            }

            if(jobj.keys().contains("Download List")) {
                QJsonValue jvalue = jobj.value("Download List");
                if(jvalue.isArray()) {
                    m_downloadIdList.clear();
                    QJsonArray jarray = jvalue.toArray();
                    int i = 0;
                    for(i = 0; i < jarray.count(); i++) {
                        QJsonValue jmodelVar = jarray.at(i);
                        if(jmodelVar.isObject()) {
                            QJsonObject jmodelobj =  jmodelVar.toObject();
                            QString id;
                            if(jmodelobj.keys().contains("id")) {
                                id = jmodelobj.value("id").toString();
                                m_downloadIdList.append(id);
                            }
                        }
                    }
                }

            }

            if(jobj.keys().contains("Download Progress")) {

                qDebug() << "Download Progress";
                QJsonValue jvalue = jobj.value("Download Progress");
                if(jvalue.isObject()) {
                    QJsonObject jproobj = jvalue.toObject();
                    qint64 total = jproobj.value("Total Data").toInt();
                    qint64 received = jproobj.value("Received Data").toInt();
                    qint64 radius = 100;

                    qDebug() << "Total" << QString::number(total) << "/" << QString::number(received);
                    qint64 var = (received*radius / total);

                    qDebug() << QString::number(var);
                    if(m_downloadIdList.count() > 0) {
                        int index = GetIndexById(m_downloadIdList.first());
                        m_pModelSelectionPane->SetProgressBarByIndex(index, var);
                    } else {
                        LogManagerInst << LogManager::LogDegreeType_Error << "Download Progress list 0, can't find current download item";
                        qDebug() << "Download list 0, can't find current download item";
                    }
                }
            }

            if(jobj.keys().contains("Download Finished Result")) {
                QJsonValue jvalue = jobj.value("Download Finished Result");
                if(jvalue.isObject()) {
                    QJsonObject jproobj = jvalue.toObject();
                    bool rst = jproobj.value("Result").toBool();
                    QString data = jproobj.value("Reply").toString();

                    if(m_downloadIdList.count() == 0) {
                        LogManagerInst << LogManager::LogDegreeType_Error << "Download Finished list 0, can't find current download item";
                        qDebug() << "Download list 0, can't find current download item";
                    }

                    int index = GetIndexById(m_downloadIdList.first());
                    if(-1 != index && rst) {
                        OnDownloadFinished(index);
                    }

                    if(!rst)
                        LogManagerInst << LogManager::LogDegreeType_Normal << "Download" << (rst?QString("Success"):QString("Failed")) << ":" << data;

                    m_downloadIdList.takeFirst();
                }

            }

            if(jobj.keys().contains("Login Finished Result")) {
                QJsonValue jvalue = jobj.value("Login Finished Result");
                if(jvalue.isObject()) {
                    QJsonObject jproobj = jvalue.toObject();
                    bool rst = jproobj.value("Result").toBool();
                    QString data = jproobj.value("Reply").toString();
                    qDebug() << "Login : " << (rst?QString("Success"):QString("Failed")) << ":" << data;
                    LogManagerInst << LogManager::LogDegreeType_Normal << "Login" << (rst?QString("Success"):QString("Failed")) << ":" << data;

                    if(rst) {
                        // request list info
                        m_pbBinder.sendToServiceMessage(QByteArray ("Request Model List"));
                    }
                }

            }
        }
    } else {
#if 0
//        qDebug() << "Message Error :" << error.errorString().toUtf8().constData();
//        qFatal(error.errorString().toUtf8().constData());
#endif

        m_modelList[m_curRequesetThumbnailIndex].thumbnail.append(data);
        m_curThumbnailSize += data.size();
        if(m_curThumbnailSize == m_modelList[m_curRequesetThumbnailIndex].thumbnailSize) {
            LogManagerInst << LogManager::LogDegreeType_Normal << m_modelList[m_curRequesetThumbnailIndex].id << "Thumbnail Success Total Size : " << QString::number(m_modelList[m_curRequesetThumbnailIndex].thumbnailSize);
            qDebug() << m_modelList[m_curRequesetThumbnailIndex].id << "Thumbnail Success Total Size : " << QString::number(m_modelList[m_curRequesetThumbnailIndex].thumbnailSize);

            m_curRequesetThumbnailIndex++;
            RequesetThumbnail();

            if(m_curRequesetThumbnailIndex >= m_modelList.count()) {
                // thumbnails success. install models
                QList<ItemBase*> modelList;
                m_itemModelList.clear();
                foreach(ModelInfo info, m_modelList) {
                    ItemModel *pItem = new ItemModel();
                    QPixmap icon;
                    bool rst = TryConvetPixmapFromData(icon, info.thumbnail);
                    LogManagerInst << LogManager::LogDegreeType_Normal << "Thumbnail Convert Image Result :" << (rst ? "Success" : "Fail");
                    pItem->SetIconImage(icon);
                    pItem->SetText(info.name);
                    pItem->SetProgressBar(0);
                    pItem->SetProgressBarVisible(false);
                    pItem->SetButtonText("Download");

                    modelList.append(pItem);
                    m_itemModelList.append(pItem);
                }
                m_pModelSelectionPane->AddItems(modelList);
                m_pModelSelectionPane->show();

                LogManagerInst << LogManager::LogDegreeType_Normal << "Install Models";
            }
        } else {
            qDebug() << "Total Size : " << QString::number(m_modelList[m_curRequesetThumbnailIndex].thumbnailSize) << " Cur Size : " << QString::number(m_curThumbnailSize);
            LogManagerInst << LogManager::LogDegreeType_Normal << "Total Size : " << QString::number(m_modelList[m_curRequesetThumbnailIndex].thumbnailSize) << " Cur Size : " << QString::number(m_curThumbnailSize);
        }
    }
}

void ModelSelection::Connect(const HostInfo& info)
{
    m_pbBinder.SetBinderAuto(info.autoBinder);
    m_pbBinder.SetBinderAddress(info.ipAddress);
    m_pbBinder.SetPortValue(info.port);

    m_pbBinder.Connect();
}

int ModelSelection::GetIndexById(const QString& id)
{
    int i = 0;
    for(i = 0;i < m_modelList.count(); i++) {
        if(id == m_modelList[i].id )
            return i;
    }

    return -1;
}

void ModelSelection::RequesetThumbnail()
{
    if(DeltaXBinder::ConnectState_Connect != m_currentConnectState)
        return;

    if(m_curRequesetThumbnailIndex >= m_modelList.count())
        return;

    m_curThumbnailSize = 0;
    m_pbBinder.sendToServiceMessage( QByteArray("Requeset thumbnail id=").append( m_modelList[m_curRequesetThumbnailIndex].id ) );

    LogManagerInst << LogManager::LogDegreeType_Normal << "Requeset thumbnail" << m_modelList[m_curRequesetThumbnailIndex].id;
}

void ModelSelection::OnModelSelectionPaneItemPressed(int index)
{
    if(DeltaXBinder::ConnectState_Connect != m_currentConnectState)
        return;

    if(index >= m_modelList.count())
        return;

    // whether load scene
    if(m_modelList[index].download == DownloadState_Downloaded) {
        QByteArray requestData("Load Scene id=");
        requestData.append( m_modelList[index].id );

        m_pbBinder.sendToServiceMessage(requestData);
    } else if(m_modelList[index].download == DownloadState_Invalid) {
        QByteArray requestData("Download content id=");
        requestData.append( m_modelList[index].id );

        m_modelList[index].download = DownloadState_Downloading;

        m_itemModelList[index]->SetProgressBarVisible(true);
        m_itemModelList[index]->SetProgressBar(0);
        m_itemModelList[index]->SetButtonText("Download");

        m_pbBinder.sendToServiceMessage(requestData);
    }
}


void ModelSelection::OnDownloadFinished(int index)
{
    m_modelList[index].download = DownloadState_Downloaded;

    m_itemModelList[index]->StartProgressBarFade();

    m_itemModelList[index]->SetButtonText("Load");
    m_itemModelList[index]->SetProgressBar(100);
}

void ModelSelection::SetTestButtonBackOff()
{
    ui->testlogin->setIcon(QIcon());
    ui->testlogin->setText("Test");
    ui->testlogin->setStyleSheet(
                "QPushButton {\n"
                "    border: 1px solid #8f8f91;\n"
                "	background-color: rgb(125, 154, 172);\n"
                "    min-width: 80px;\n"
                "	\n"
                "	font: 10pt \"MS Shell Dlg 2\";\n"
                "}\n"
                "\n"
                "QPushButton:pressed {\n"
                "	border: 1px solid #8f8f91;\n"
                "	background-color: rgb(117, 151, 172);\n"
                "}"
                                );
}

bool ModelSelection::TryConvetPixmapFromData(QPixmap& pixmap, const QByteArray& data)
{
    QPixmap tmpPixmap;
    bool convertRst = false;
    const static char* formatArray[] = {
            "png"
        ,   "jpg"
        ,   "bmp"

    };

    convertRst = tmpPixmap.loadFromData(data);
    if(convertRst) {
        pixmap = tmpPixmap;
        return convertRst;
    }

    int i = 0;
    int formatCount = sizeof(formatArray) / sizeof(formatArray[0]);
    for(i = 0; i < formatCount; i++) {
        convertRst = tmpPixmap.loadFromData(data, formatArray[i]);
        if(convertRst) {
            pixmap = tmpPixmap;
            return convertRst;
        }
    }

    return convertRst;
}

QByteArray ModelSelection::GetCurrentLoginInfoData()
{
    QByteArray ret;
    QJsonObject jobj;
    QJsonObject jLogin;

    if( (m_curInfoIndex > -1) && (m_curInfoIndex < PbInfoList.count()) ) {
        jobj.insert("user", PbInfoList[m_curInfoIndex].user);
        jobj.insert("password", PbInfoList[m_curInfoIndex].password);
    }

    jLogin.insert("Requeset Login", jobj);
    ret = QJsonDocument(jLogin).toJson();

    return ret;
}
