#include "modelselectionex.h"
#include "ui_modelselectionex.h"
#include "deletedialogex.h"
#include "logmanager.h"
#include "resourcemanager.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonParseError>

#include <QEvent>
#include <QScrollArea>
#include <QMouseEvent>
#include <QMovie>
#include <QScrollBar>
#include <QTime>
#include <QDebug>

#define ITEM_SPACE_HEIGHT   (37)
#define ITEM_LEFT     (33)

ModelSelectionEx::ModelSelectionEx(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ModelSelectionEx)
    , m_currentConnectState(DeltaXBinder::ConnectState_Invalid)
    , m_curRequesetThumbnailIndex(-1)
    , m_curThumbnailSize(0)
    , m_curSetId(SetId_Invalid)
    , m_curInfoIndex(-1)
    , m_testing(false)
    , m_signalMap(this)
{
    ui->setupUi(this);
    ui->comboBox->clear();
    pScrollArea = new QScrollArea(ui->modelpane);
    pScrollArea->resize(ui->modelpane->size());
    pScrollArea->installEventFilter(this);
    QMovie* pMovie = new QMovie(":/configure/Resource/configure/u1184.gif");
    ui->refreshingLabel->setMovie(pMovie);
    ui->refreshingLabel->setContentsMargins(15,0,0,0);
    pMovie->start();
    ui->refreshingLabel->hide();
    init();
}

ModelSelectionEx::~ModelSelectionEx()
{
    pScrollArea->removeEventFilter(this);
    final();

    delete ui;
}

void ModelSelectionEx::init()
{
    // use deleteDialogEx instead of deletepane
    ui->deletepane->hide();
    ui->setpane->hide();

    ui->titleedit->installEventFilter(this);
    ui->serveredit->installEventFilter(this);
    ui->portedit->installEventFilter(this);
    ui->useredit->installEventFilter(this);
    ui->passwordedit->installEventFilter(this);

    QStringList titleList;
    foreach(PbInfo pbInfo, PbInfoList) {
        titleList.append(pbInfo.title);
    }
    ui->comboBox->addItems(titleList);
    // ...
    if(titleList.count() > 0)
        m_curInfoIndex = 0;

    connect(&m_pbBinder, SIGNAL(BinderStatusChanged(DeltaXBinder::ConnectState)), this, SLOT(OnConnectStatusChanged(DeltaXBinder::ConnectState)));
    connect(&m_pbBinder, SIGNAL(DataReceived(QByteArray)), this, SLOT(OnDataReceived(QByteArray)));

    m_itemsPane.installEventFilter(this);
}

void ModelSelectionEx::final()
{
    ui->titleedit->removeEventFilter(this);
    ui->serveredit->removeEventFilter(this);
    ui->portedit->removeEventFilter(this);
    ui->useredit->removeEventFilter(this);
    ui->passwordedit->removeEventFilter(this);

    disconnect(&m_pbBinder, SIGNAL(BinderStatusChanged(DeltaXBinder::ConnectState)), this, SLOT(OnConnectStatusChanged(DeltaXBinder::ConnectState)));
    disconnect(&m_pbBinder, SIGNAL(DataReceived(QByteArray)), this, SLOT(OnDataReceived(QByteArray)));

    {
        QScrollBar* pScrollBar = pScrollArea->verticalScrollBar();
        disconnect(pScrollBar, SIGNAL(valueChanged(int)), this, SLOT(OnSliderValueChanged(int)));
        disconnect(pScrollBar, SIGNAL(sliderPressed()), this, SLOT(OnSliderPressed()));
        pScrollBar->removeEventFilter(this);
    }

    m_itemsPane.removeEventFilter(this);
}

bool ModelSelectionEx::eventFilter(QObject *obj, QEvent *event)
{
    static int s_moveBeginY = 0;
    static int s_beginValue = 0;
    static bool s_flipBegin = false;
    static QTime s_flipTime;
    QScrollBar* pScrollBar = pScrollArea->verticalScrollBar();

    if( obj == ui->titleedit || obj == ui->serveredit || obj == ui->portedit ||
            obj == ui->useredit || obj == ui->passwordedit) {
        // LogManagerInst << LogManager::LogDegreeType_Warn << QString::number(event->type());
        if(event->type() == QEvent::MouseButtonPress) {
            CommonUtils::ShowTabTip();
        }
    }

    if(obj == &m_itemsPane) {
        int innerPaneHeight = m_itemsPane.height();
        int maxValue = pScrollBar->maximum();
        int minValue = pScrollBar->minimum();
        bool vertialScrollBarVisible = pScrollBar->isVisible();

        if(vertialScrollBarVisible) {
            switch(event->type()) {
            case QEvent::MouseButtonPress: {
                QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
                s_moveBeginY = mouseEvent->globalY();
                s_beginValue = pScrollBar->value();
            }
                break;
            case QEvent::MouseMove: {
                if(m_listAnimationManager.OnRunning()) {
                    m_listAnimationManager.RemoveAllAnimation();
                }

                QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
                int deltaY = mouseEvent->globalY() - s_moveBeginY;
                int value = int((float)(deltaY) / innerPaneHeight * (maxValue - minValue));
                value = s_beginValue - value;
                pScrollBar->setValue(value);

                if( !s_flipBegin ) {
                    s_flipTime.start();
                }
                s_flipBegin = true;
            }
                break;
            case QEvent::MouseButtonRelease: {
                QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
                int deltaY = mouseEvent->globalY() - s_moveBeginY;
                int elapsedTime = s_flipTime.elapsed();
                s_flipBegin = false;

                if(elapsedTime < 0.01)
                    elapsedTime = 1;

                // speed check to get target scrollbar value
                float speed = (float)deltaY / elapsedTime;
                const float uSpeed = qAbs(speed);
                bool flipAnimationOn = false;
                uint intervalTime = 0;
                int animationDeltaY = 0;
                int value = 0;

                if(uSpeed < 0.7) {
                    flipAnimationOn = false;
                } else if(uSpeed < 1.2) {
                    flipAnimationOn = true;
                    intervalTime = 1280 * uSpeed;
                    animationDeltaY = speed * intervalTime / 5;
                    value = int((float)(animationDeltaY) / innerPaneHeight * (maxValue - minValue));
                    value = s_beginValue - value;
                } else{
                    flipAnimationOn = true;
                    intervalTime = 1280 * uSpeed;
                    animationDeltaY = speed * intervalTime / 2;
                    value = int((float)(animationDeltaY) / innerPaneHeight * (maxValue - minValue));
                    value = s_beginValue - value;
                }

                if( value < minValue ) {
                    intervalTime = (float)intervalTime * qAbs(pScrollBar->value() - minValue)/qAbs(pScrollBar->value() - value);
                    value = minValue;
                } else if( value > maxValue ) {
                    intervalTime = (float)intervalTime * qAbs(pScrollBar->value() - maxValue)/qAbs(pScrollBar->value() - value);
                    value = maxValue;
                } else {

                }

                QEasingCurve::Type animationType = QEasingCurve::OutCirc;

                // animation
                if(flipAnimationOn){
                    m_listAnimationManager.AddAnimation(pScrollBar, UIAnimationManager::ActionType_ScrollBar_Value, pScrollBar->value(), value);
                    m_listAnimationManager.SetDuration(intervalTime);
                    m_listAnimationManager.SetEasingCurve(animationType);
                    m_listAnimationManager.StartAnimation();
                }
            }
                break;
            default:
                break;
            }
        }
    } else if(obj == pScrollBar) {
        switch(event->type()) {
        case QEvent::Wheel:
        case QEvent::MouseButtonPress:
            m_listAnimationManager.RemoveAllAnimation();
            break;
        default:
            break;
        }

    } else if(obj == pScrollArea ) {
        switch(event->type()) {
        case QEvent::Wheel:
        case QEvent::MouseButtonPress:
            m_listAnimationManager.RemoveAllAnimation();
            break;
        default:
            break;
        }
    } else {

    }

    return obj->event(event);
}

void ModelSelectionEx::on_refreshBtn_clicked()
{
    m_curSetId = SetId_Update;

    CurrentInfoConnect(m_curInfoIndex);
}

void ModelSelectionEx::on_addBtn_clicked()
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

void ModelSelectionEx::on_editbtn_clicked()
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

void ModelSelectionEx::on_deleteBtn_clicked()
{
    m_curSetId = SetId_Delete;

    if(m_curInfoIndex == -1)
        return;

    DeleteDialogEx diag(this);
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

//        m_pModelSelectionPane->RemoveElements();
//        m_pModelSelectionPane->update();
    }
}

void ModelSelectionEx::on_testlogin_clicked()
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
                "	font: 12pt \"MS Shell Dlg 2\";\n"
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

void ModelSelectionEx::on_submitBtn_clicked()
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

//    CurrentInfoConnect(m_curInfoIndex);

    ui->setpane->hide();

    m_curSetId = SetId_Invalid;
}

void ModelSelectionEx::on_cancelBtn_clicked()
{
    m_curSetId = SetId_Invalid;
    ui->setpane->hide();
}

void ModelSelectionEx::on_comboBox_activated(int index)
{
//    CurrentInfoConnect(index);

    if(index >= PbInfoList.size() || index < 0)  {
        return;
    }

    m_curInfoIndex = index;
    ui->refreshBtn->setIcon(QIcon(":/configure/Resource/configure/u1184.png"));

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

void ModelSelectionEx::CurrentInfoConnect(int index)
{
    if(index >= PbInfoList.size() || index < 0)  {
        return;
    }

    m_curInfoIndex = index;

    ui->refreshBtn->setIcon(QIcon(":/configure/Resource/configure/u1205.png"));
    ui->refreshingLabel->show();

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

void ModelSelectionEx::OnConnectStatusChanged(DeltaXBinder::ConnectState state)
{
    m_currentConnectState = state;

    if(DeltaXBinder::ConnectState_Connect != m_currentConnectState) {
        ui->refreshBtn->setIcon(QIcon(":/configure/Resource/configure/u1205.png"));
        ui->refreshingLabel->hide();
        return;
    }

    m_pbBinder.sendToServiceMessage(GetCurrentLoginInfoData());
}

void ModelSelectionEx::HandleTestLoginResult(bool ret)
{
    m_testing = false;
    if(ret) {
        ui->testlogin->setIcon(QIcon(":/configure/Resource/configure/u394.png"));
        ui->testlogin->setText("Successed");
        ui->testlogin->setStyleSheet("border:0px;"
                                     "	font: 12pt \"MS Shell Dlg 2\";\n");
    } else {
        ui->testlogin->setIcon(QIcon(":/configure/Resource/configure/u392.png"));
        ui->testlogin->setText("Failed");
        ui->testlogin->setStyleSheet("border:0px;"
                                     "	font: 12pt \"MS Shell Dlg 2\";\n");
    }
}

void ModelSelectionEx::OnDataReceived(const QByteArray& data)
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
                            info.download = ModelState_Invalid;
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
                        m_itemModelList[index]->SetProgressBar(var);
  //                      m_pModelSelectionPane->SetProgressBarByIndex(index, var);
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

                    if(m_testing) {
                        HandleTestLoginResult(rst);
                    } else {
                        if(rst) {
                            // request list info
                            m_pbBinder.sendToServiceMessage(QByteArray ("Request Model List"));
                        } else {
                            ui->refreshingLabel->hide();
                        }
                    }
                }

            }

            if(jobj.keys().contains("Load Scene Reply")) {
                QJsonValue jvalue = jobj.value("Load Scene Reply");
                if(jvalue.isObject()) {
                    QJsonObject jreplyObj = jvalue.toObject();
//                    QString name = jreplyObj.value("Name").toString();
                    QString contentId = jreplyObj.value("ContentId").toString();
                    bool result = jreplyObj.value("Result").toBool();

 //                   OnLoadSceneReplyUpdate(result, name);
                    int index = GetIndexById(contentId);
                    if(index != -1) {
                        m_itemModelList[index]->SetButtonText(result?"OPENED":"FAILED");
                        m_modelList[index].download = ModelState_Opened;
                    }
                }
            }
        }
    } else {
 //       qDebug() << "ModelList Count : " << QString::number(m_modelList.count()) << " " << QString::number(m_curRequesetThumbnailIndex);
        if(m_curRequesetThumbnailIndex >= m_modelList.count())
            return;

        m_modelList[m_curRequesetThumbnailIndex].thumbnail.append(data);
        m_curThumbnailSize += data.size();
        if(m_curThumbnailSize == m_modelList[m_curRequesetThumbnailIndex].thumbnailSize) {
            LogManagerInst << LogManager::LogDegreeType_Normal << m_modelList[m_curRequesetThumbnailIndex].id << "Thumbnail Success Total Size : " << QString::number(m_modelList[m_curRequesetThumbnailIndex].thumbnailSize);
            qDebug() << m_modelList[m_curRequesetThumbnailIndex].id << "Thumbnail Success Total Size : " << QString::number(m_modelList[m_curRequesetThumbnailIndex].thumbnailSize);

            m_curRequesetThumbnailIndex++;
            RequesetThumbnail();

            if(m_curRequesetThumbnailIndex >= m_modelList.count()) {
                // thumbnails success. install models
                QList<ItemModelEx*> modelList;
                FinalItemsSignals();
                foreach(ItemModelEx* pItem, m_itemModelList) {
                    delete pItem;
                }
                m_itemModelList.clear();
                foreach(ModelInfo info, m_modelList) {
                    ItemModelEx *pItem = new ItemModelEx(&m_itemsPane);
                    QPixmap icon;
                    bool rst = TryConvetPixmapFromData(icon, info.thumbnail);
                    LogManagerInst << LogManager::LogDegreeType_Normal << "Thumbnail Convert Image Result :" << (rst ? "Success" : "Fail");
                    pItem->SetIconImage(icon);
                    pItem->SetText(info.name);
                    pItem->SetProgressBar(0);
                    pItem->SetProgressBarVisible(false);
                    pItem->SetButtonText("Download");
                    pItem->SetDownloadStateWidgetText(" ");

                    modelList.append(pItem);
                    m_itemModelList.append(pItem);
                }
                InitItemsSignals();
                SetModels(modelList);

                ui->refreshingLabel->hide();
                ui->refreshBtn->setIcon(QIcon(":/configure/Resource/configure/u1201.png"));

                LogManagerInst << LogManager::LogDegreeType_Normal << "Install Models";
            }
        } else {
            qDebug() << "Total Size : " << QString::number(m_modelList[m_curRequesetThumbnailIndex].thumbnailSize) << " Cur Size : " << QString::number(m_curThumbnailSize);
            LogManagerInst << LogManager::LogDegreeType_Normal << "Total Size : " << QString::number(m_modelList[m_curRequesetThumbnailIndex].thumbnailSize) << " Cur Size : " << QString::number(m_curThumbnailSize);
        }
    }
}

void ModelSelectionEx::SetModels(QList<ItemModelEx*> list)
{
    if(list.isEmpty())
        return;

    const int listCount = list.count();
    const int itemHeight = list[0]->height();
    const int paneHeight = (itemHeight + ITEM_SPACE_HEIGHT) * listCount + ITEM_SPACE_HEIGHT;
    const int paneWidth = ui->modelpane->width() - 30;

    m_itemsPane.resize(paneWidth, paneHeight);
    int i = 0;
    for(i = 0; i < listCount; i++) {
        list[i]->move(ITEM_LEFT, itemHeight * i + ITEM_SPACE_HEIGHT * (i+1) );
        list[i]->show();
    }

//    QScrollArea* pScrollArea = new QScrollArea;
    pScrollArea->resize(ui->modelpane->size());
    pScrollArea->setWidget(&m_itemsPane);
    pScrollArea->show();

    {
        QScrollBar* pScrollBar = pScrollArea->verticalScrollBar();
        connect(pScrollBar, SIGNAL(valueChanged(int)), this, SLOT(OnSliderValueChanged(int)));
        connect(pScrollBar, SIGNAL(sliderPressed()), this, SLOT(OnSliderPressed()));
        pScrollBar->installEventFilter(this);
    }
}


void ModelSelectionEx::InitItemsSignals()
{
    int i = 0;
    for(i = 0; i < m_itemModelList.count(); i++) {
        ItemModelEx* pModel = m_itemModelList[i];
        connect(pModel, SIGNAL(ButtonPressed()), &m_signalMap, SLOT(map()));
        m_signalMap.setMapping(pModel, i);
    }
    connect(&m_signalMap, SIGNAL(mapped(int)), this, SLOT(OnItemButtonPressed(int)));
}

void ModelSelectionEx::FinalItemsSignals()
{
    int i = 0;
    for(i = 0; i < m_itemModelList.count(); i++) {
        ItemModelEx* pModel = m_itemModelList[i];
        disconnect(pModel, SIGNAL(ButtonPressed()), &m_signalMap, SLOT(map()));
    }
    disconnect(&m_signalMap, SIGNAL(mapped(int)), this, SLOT(OnItemButtonPressed(int)));
}

void ModelSelectionEx::Connect(const HostInfo& info)
{
    m_pbBinder.SetBinderAuto(info.autoBinder);
    m_pbBinder.SetBinderAddress(info.ipAddress);
    m_pbBinder.SetPortValue(info.port);

    m_pbBinder.Connect();
}

int ModelSelectionEx::GetIndexById(const QString& id)
{
    int i = 0;
    for(i = 0;i < m_modelList.count(); i++) {
        if(id == m_modelList[i].id )
            return i;
    }

    return -1;
}

void ModelSelectionEx::RequesetThumbnail()
{
    if(DeltaXBinder::ConnectState_Connect != m_currentConnectState)
        return;

    if(m_curRequesetThumbnailIndex >= m_modelList.count())
        return;

    m_curThumbnailSize = 0;
    m_pbBinder.sendToServiceMessage( QByteArray("Requeset thumbnail id=").append( m_modelList[m_curRequesetThumbnailIndex].id ) );

    LogManagerInst << LogManager::LogDegreeType_Normal << "Requeset thumbnail" << m_modelList[m_curRequesetThumbnailIndex].id;
}

void ModelSelectionEx::OnItemButtonPressed(int index)
{
    if(DeltaXBinder::ConnectState_Connect != m_currentConnectState)
        return;

    if(index >= m_modelList.count())
        return;

    static int lastIndex = -1;
    static int sameCounter = 0;

    if(lastIndex == index) {
        sameCounter++;
    } else {
        lastIndex = index;
        sameCounter = 0;
    }

    if(sameCounter >= 3) {
        // show tips
        return;
    }

    // whether load scene
    if(m_modelList[index].download == ModelState_Downloaded || m_modelList[index].download == ModelState_Opened) {
        QByteArray requestData("Load Scene id=");
        requestData.append( m_modelList[index].id );

        m_modelList[index].download = ModelState_Opening;

        m_itemModelList[index]->SetProgressBarVisible(false);
        m_itemModelList[index]->SetDownloadWidgetVisible(true);
        m_itemModelList[index]->SetButtonText("OPENING");

        m_pbBinder.sendToServiceMessage(requestData);
    } else if(m_modelList[index].download == ModelState_Invalid) {
        QByteArray requestData("Download content id=");
        requestData.append( m_modelList[index].id );

        m_modelList[index].download = ModelState_Downloading;

        m_itemModelList[index]->SetProgressBarVisible(true);
        m_itemModelList[index]->SetProgressBar(0);
        m_itemModelList[index]->SetDownloadWidgetVisible(false);
 //       m_itemModelList[index]->SetButtonText("Download");

        m_pbBinder.sendToServiceMessage(requestData);
    }
}


void ModelSelectionEx::OnDownloadFinished(int index)
{
    m_modelList[index].download = ModelState_Downloaded;

#if 0
    m_itemModelList[index]->StartProgressBarFade();
    m_itemModelList[index]->SetButtonText("Load");
    m_itemModelList[index]->SetProgressBar(100);
#endif
    m_itemModelList[index]->SetProgressBarVisible(false);
    m_itemModelList[index]->SetDownloadWidgetVisible(true);
    m_itemModelList[index]->SetDownloadStateWidgetText("Downloaded");
    m_itemModelList[index]->SetButtonText("OPEN");
}

void ModelSelectionEx::SetTestButtonBackOff()
{
    ui->testlogin->setIcon(QIcon());
    ui->testlogin->setText("Test");
    ui->testlogin->setStyleSheet(
                "QPushButton {\n"
                "    border: 1px solid #8f8f91;\n"
                "	background-color: rgb(125, 154, 172);\n"
                "    min-width: 80px;\n"
                "	\n"
                "	font: 12pt \"MS Shell Dlg 2\";\n"
                "}\n"
                "\n"
                "QPushButton:pressed {\n"
                "	border: 1px solid #8f8f91;\n"
                "	background-color: rgb(117, 151, 172);\n"
                "}"
                                );
}

bool ModelSelectionEx::TryConvetPixmapFromData(QPixmap& pixmap, const QByteArray& data)
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

QByteArray ModelSelectionEx::GetCurrentLoginInfoData()
{
    QByteArray ret;
    QJsonObject jobj;
    QJsonObject jLogin;

    if( (m_curInfoIndex > -1) && (m_curInfoIndex < PbInfoList.count()) ) {
        if(m_testing) {
            jobj.insert("user", ui->useredit->text());
            jobj.insert("password", ui->passwordedit->text());
        } else {
            jobj.insert("user", PbInfoList[m_curInfoIndex].user);
            jobj.insert("password", PbInfoList[m_curInfoIndex].password);
        }
        jobj.insert("only login", m_testing);
    }

    jLogin.insert("Requeset Login", jobj);
    ret = QJsonDocument(jLogin).toJson();

    return ret;
}

void ModelSelectionEx::OnLoadSceneReplyUpdate(bool result, const QString& name)
{
    int i = 0;
    int index = -1;
    for(i = 0;i < m_modelList.count(); i++) {
        if(name == m_modelList[i].modelname ) {
            index = i;
            break;
        }
    }

    if(index != -1) {
        m_itemModelList[index]->SetButtonText(result?"OPENED":"FAILED");
        m_modelList[index].download = ModelState_Opened;
    }
}

void ModelSelectionEx::OnSliderValueChanged(int)
{
#ifdef DEBUG
    QScrollBar* pScrollBar = pScrollArea->verticalScrollBar();
    int maxValue = pScrollBar->maximum();
    int minValue = pScrollBar->minimum();
    qDebug() << "Max : " << QString::number(maxValue) << " , Min : " << QString::number(minValue) << " , CurValue : " << QString::number(value);
#endif
}

void ModelSelectionEx::OnSliderPressed()
{
    m_listAnimationManager.RemoveAllAnimation();
}
