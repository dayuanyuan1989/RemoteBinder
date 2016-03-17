#include "mainpane.h"
#include "ui_mainpane.h"

#include <windows.h>
#include <QMoveEvent>
#include "resourcemanager.h"
#include <QDebug>

#include <QMouseEvent>

MainPane::MainPane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainPane),
    m_currentConnectState(DeltaXBinder::ConnectState_Disconnect),
    m_curModeType(ModeType_Preview),
    m_liveView(true),
    m_pRenderOnScreen(NULL)
{
    ui->setupUi(this);

    init();
}

MainPane::~MainPane()
{
    final();
    delete ui;
}

void MainPane::init()
{
    m_pModeGrp = new QButtonGroup(this);
    m_pModeGrp->addButton(ui->previewbtn, 0);
    m_pModeGrp->addButton(ui->plorebtn, 1);
    m_pModeGrp->addButton(ui->configurebtn, 2);
    connect(m_pModeGrp, SIGNAL(buttonPressed(int)), this, SLOT(onModeChanged(int)));

    m_pSetHelpDiag = new SettingHelpDiag(this);
    m_pSetHelpDiag->raise();
    m_pSetHelpDiag->move(1505, 110);
    m_pSetHelpDiag->hide();

    connect(m_pSetHelpDiag, SIGNAL(CheckBoxChanged(int, bool)), this, SLOT(OnHelpButtonPressed(int, bool)));
    connect(m_pSetHelpDiag, SIGNAL(ServerReConnect(int)), this, SLOT(OnSetButtonPressed(int)));

    m_pAddJobDiag = new AddJobDiag(this);
    connect(m_pAddJobDiag, SIGNAL(AddNewTask(const RenderData&)), this, SLOT(OnAddNewRenderTask(const RenderData&)));
    m_pAddJobDiag->raise();
    m_pAddJobDiag->move(5, 112);
    m_pAddJobDiag->hide();

    m_pJobListDiag = new JobListDiag(this);
    connect(m_pJobListDiag, SIGNAL(ButtonClicked()), this, SLOT(OnStartTaskRender()));
    m_pJobListDiag->raise();
    m_pJobListDiag->move(5, 112);
    m_pJobListDiag->hide();

    m_pModelSelection = new ModelSelectionEx(this);
    m_pModelSelection->raise();
    m_pModelSelection->move(800, 112);
    m_pModelSelection->hide();
//    connect(this, SIGNAL(LoadSceneReplyUpdate(bool,QString)), m_pModelSelection, SLOT(OnLoadSceneReplyUpdate(bool,QString)));

    ui->titlewidget->installEventFilter(this);

    m_pVariantPane = new VariantSetsPane(ui->varsetpane);
    m_pTouchPane = new TouchPane(ui->preivewpane);
    ui->previewboot->raise();
    ui->grphelpbox->raise();
    ui->previewboot->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->grphelpbox->setAttribute(Qt::WA_TransparentForMouseEvents);

    connect(m_pVariantPane, SIGNAL(VariantSetUpdate(QString)), this, SLOT(OnDataTransfer(QString)));
    connect(m_pTouchPane, SIGNAL(MousePaneUpdate(QString)), this, SLOT(OnMouseDataTransfer(QString)));

    m_pWaterMarkWidget = new WaterMarkWidget(m_pTouchPane);
    connect(m_pAddJobDiag, SIGNAL(WaterMarkVisibleChanged(bool)), m_pWaterMarkWidget, SLOT(waterMarkVisibleChanged(bool)));
    connect(m_pAddJobDiag, SIGNAL(fontFamilyChanged(QFont)), m_pWaterMarkWidget, SLOT(fontFamilyChanged(QFont)));
    connect(m_pAddJobDiag, SIGNAL(fontSizeChanged(uint)), m_pWaterMarkWidget, SLOT(fontSizeChanged(uint)));
    connect(m_pAddJobDiag, SIGNAL(BoldStateChanged(bool)), m_pWaterMarkWidget, SLOT(BoldStateChanged(bool)));
    connect(m_pAddJobDiag, SIGNAL(ItalyStateChanged(bool)), m_pWaterMarkWidget, SLOT(ItalyStateChanged(bool)));
    connect(m_pAddJobDiag, SIGNAL(TextColorChanged(QColor)), m_pWaterMarkWidget, SLOT(TextColorChanged(QColor)));
    connect(m_pAddJobDiag, SIGNAL(TextChanged(QString)), m_pWaterMarkWidget, SLOT(TextChanged(QString)));
 //   m_pWaterMarkWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
 //   m_pWaterMarkWidget->setAttribute(Qt::WA_TranslucentBackground);
    m_pWaterMarkWidget->TextChanged("Sample Text");
    m_pWaterMarkWidget->move(400, 400);
    m_pWaterMarkWidget->fontSizeChanged(36);
    m_pWaterMarkWidget->hide();

    // binder
    connect(&m_dgBinder, SIGNAL(DataReceived(QByteArray)), this, SLOT(OnDataReceived(QByteArray)));
    connect(&m_dgBinder, SIGNAL(BinderStatusChanged(DeltaXBinder::ConnectState)), this, SLOT(OnConnectStatusChanged(DeltaXBinder::ConnectState)));

//    connect(&m_pbBinder, SIGNAL(DataReceived(QByteArray)), this, SLOT(OnPbDataReceived(QByteArray)));
//    connect(&m_pbBinder, SIGNAL(BinderStatusChanged(DeltaXBinder::ConnectState)), this, SLOT(OnPbConnectStatusChanged(DeltaXBinder::ConnectState)));

    // body
    m_previewWinId = GetPreviewWinId();

    m_pVariantPane->resize(ui->varsetpane->size());
    m_pTouchPane->resize(ui->preivewpane->size());

    ModeTypeChanged(m_curModeType);
    GeometryAdjust();

    m_pRenderOnScreen = new RenderOnScreen(this);
    m_pRenderOnScreen->move(0, 0);
    m_pRenderOnScreen->raise();
    m_pRenderOnScreen->hide();
}

void MainPane::final()
{
    if(m_pSetHelpDiag) {
        disconnect(m_pSetHelpDiag, SIGNAL(CheckBoxChanged(int, bool)), this, SLOT(OnHelpButtonPressed(int, bool)));
        disconnect(m_pSetHelpDiag, SIGNAL(ServerReConnect(int)), this, SLOT(OnSetButtonPressed(int)));
    }

    disconnect(m_pAddJobDiag, SIGNAL(AddNewTask(const RenderData&)), this, SLOT(OnAddNewRenderTask(const RenderData&)));
    disconnect(m_pJobListDiag, SIGNAL(ButtonClicked()), this, SLOT(OnStartTaskRender()));

    ui->titlewidget->removeEventFilter(this);

    disconnect(m_pVariantPane, SIGNAL(VariantSetUpdate(QString)), this, SLOT(OnDataTransfer(QString)));
    disconnect(m_pTouchPane, SIGNAL(MousePaneUpdate(QString)), this, SLOT(OnMouseDataTransfer(QString)));

    disconnect(&m_dgBinder, SIGNAL(DataReceived(QByteArray)), this, SLOT(OnDataReceived(QByteArray)));
    disconnect(&m_dgBinder, SIGNAL(BinderStatusChanged(DeltaXBinder::ConnectState)), this, SLOT(OnConnectStatusChanged(DeltaXBinder::ConnectState)));

//    disconnect(this, SIGNAL(LoadSceneReplyUpdate(bool,QString)), m_pModelSelection, SLOT(OnLoadSceneReplyUpdate(bool,QString)));
//    disconnect(&m_pbBinder, SIGNAL(DataReceived(QByteArray)), this, SLOT(OnPbDataReceived(QByteArray)));
 //   disconnect(&m_pbBinder, SIGNAL(BinderStatusChanged(DeltaXBinder::ConnectState)), this, SLOT(OnPbConnectStatusChanged(DeltaXBinder::ConnectState)));
}

void MainPane::moveEvent(QMoveEvent*)
{

}

void MainPane::showEvent(QShowEvent *)
{
    onModeChanged(m_curModeType);
}

void MainPane::hideEvent(QHideEvent *)
{

}

void MainPane::closeEvent(QCloseEvent *)
{

}

void MainPane::OnAddNewRenderTask(const RenderData& data)
{
    QString qualityString;
    switch(data.quality) {
    case 0:
        qualityString = "Medium";
        break;
    case 1:
        qualityString = "High";
        break;
    case 2:
        qualityString = "Very High";
        break;
    default:
        qualityString = "Medium";
    }
    m_pJobListDiag->addItem(data.fileName, QString("%1 X %2").arg(data.size.width()).arg(data.size.height()), qualityString);
    m_dgBinder.sendToServiceMessage(m_pAddJobDiag->GetAddRenderTaskString() + GetWaterMarkInfoMessage(m_pWaterMarkWidget->GetWaterMarkInfo(), data.size));
}

void MainPane::OnStartTaskRender()
{
    m_dgBinder.sendToServiceMessage("StartRenderTask");
    m_pRenderOnScreen->show();
}

void MainPane::GeometryAdjust()
{
    switch(m_curModeType) {
    case ModeType_Preview:
        ui->preivewpane->setGeometry(0, 0, ui->bodywidget->width(), ui->bodywidget->height());
        m_pTouchPane->resize(ui->preivewpane->size());
        ui->grphelpbox->move(ui->preivewpane->width() - 1 - 339, ui->preivewpane->height() - 1 - 137);
        PreviewWinGeometryAdjust();
        break;

    case ModeType_Plore:
    {
        ui->varsetpane->setGeometry(0, 142, 674, 868);
        ui->preivewpane->setGeometry(674, 142, 1246, 868);
        ui->previewboot->setGeometry(0 , ui->preivewpane->height() - ui->previewboot->height()
                                     , ui->preivewpane->width(), ui->previewboot->height());
        ui->grphelpbox->move(ui->preivewpane->width() - 1 - 339, ui->preivewpane->height() - 1 - 137);
        ui->sperator->move(673, 42);
        m_pVariantPane->resize(ui->varsetpane->size());
        m_pTouchPane->resize(ui->preivewpane->size());
        PreviewWinGeometryAdjust();
    }
        break;

    case ModeType_Configure:
    {
        ui->varsetpane->setGeometry(0, 142, 1094, 868);
        ui->preivewpane->setGeometry(1095, 142, 825, 868);
        ui->previewboot->setGeometry(0 , ui->preivewpane->height() - ui->previewboot->height()
                                     , ui->preivewpane->width(), ui->previewboot->height());
        ui->grphelpbox->move(ui->preivewpane->width() - 1 - 339, ui->preivewpane->height() - 1 - 137);
        ui->sperator->move(1094, 42);
        m_pVariantPane->resize(ui->varsetpane->size());
        m_pTouchPane->resize(ui->preivewpane->size());
    }
        break;

    default:

        break;
    }
}

void MainPane::SocketClientSetting(const QString& ipAddress, uint port, bool autoConnnect)
{
    if(!autoConnnect)   m_dgBinder.SetBinderAddress(ipAddress);
    m_dgBinder.SetBinderAuto(autoConnnect);
    m_dgBinder.SetPortValue(port);
}

void MainPane::VariantSetsBinderConnect(const HostInfo& host)
{
//    static HostInfo info;

#if 0
    if(host.ipAddress == info.ipAddress && host.port == info.port && m_currentConnectState == 1) return;
 //   warning: static value used by memcpy, when need to deconstruct, may occur error.
    info.autoBinder = host.autoBinder;
    info.ipAddress = host.ipAddress;
    info.port = host.port;
#endif

    SocketClientSetting(host.ipAddress, host.port, host.autoBinder);
    m_dgBinder.DisConnect();
    m_dgBinder.Connect();
}

HWND MainPane::GetPreviewWinId()
{
 //   return NULL;
    static HWND winId = NULL;
    HWND getWinId = FindWindow(NULL, TITLE_NAME);
    if(getWinId!=NULL) winId = getWinId;
    return winId;
}

void MainPane::CloseDgReflectWindow()
{
    if(m_previewWinId) {
        DWORD   ProcessID;
        HANDLE  hProcess;

        GetWindowThreadProcessId(m_previewWinId, &ProcessID);
        hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,ProcessID);
        TerminateProcess(hProcess,   0);
    }
}

bool MainPane::eventFilter(QObject *obj, QEvent *event)
{
    // it prepare for title to move, not used
#if 0
    if(obj == ui->titlewidget) {
        static QPoint pressedPoint;
        static QPoint pressedWidgetPoint;

        if(event->type() ==  QEvent::MouseButtonPress) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            pressedPoint = mouseEvent->globalPos();
            pressedWidgetPoint = mapToGlobal(QPoint(0, 0));
        }

        if(event->type() == QEvent::MouseMove) {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            move(pressedWidgetPoint + mouseEvent->globalPos() - pressedPoint);
        }

    }
#endif

    return obj->event(event);
}

void MainPane::PreviewWinGeometryAdjust()
{
    if(m_previewWinId == NULL) return;

    static bool enter = false;
    if(enter==false) {
        enter = true;
        LONG Style = GetWindowLong(m_previewWinId, GWL_STYLE);               //得到窗口风格
        Style = Style&(!WS_CAPTION);	//去掉标题栏
        SetWindowLong(m_previewWinId, GWL_STYLE, Style);	//为窗口设置新的风格

        // set parent into variantset, if changed mode to preview or control, then move the window, error occur
        // but if I move the window in preoview|cotrol, then chagne mode to others, move, error dismissed
        // unknow error
 //       this->move((1920-1300)/2, (1080-800)/2);
    }

    int width = ui->preivewpane->width();
    int height = ui->preivewpane->height();

    switch(m_curModeType) {
    case ModeType_Preview:
        width = ui->preivewpane->width();
        height = ui->preivewpane->height();
        break;

    case ModeType_Plore:
        width = ui->preivewpane->width();
        height = ui->preivewpane->height() - ui->previewboot->height();
        break;

    default:
        break;
    }

    MoveWindow(m_previewWinId, ui->preivewpane->mapToGlobal(QPoint(0, 0)).x(), ui->preivewpane->mapToGlobal(QPoint(0, 0)).y()
               , width, height, false);
    ShowWindow(m_previewWinId, SW_SHOWNORMAL);
}

void MainPane::ModeTypeChanged(ModeType type)
{
    switch(type) {
    case ModeType_Preview:
        m_pTouchPane->SetBackgroundTransparent(m_liveView);
        ui->preivewpane->show();
        ui->previewboot->hide();
        ui->varsetgrpbtn->hide();
        ui->plorebodylabelsgrp->hide();
        ui->configurebodylabelsgrp->hide();
        ui->varsetpane->hide();
        ui->sperator->hide();
        break;
    case ModeType_Plore:
        m_pTouchPane->SetBackgroundTransparent(m_liveView);
        ui->plorebodylabelsgrp->show();
        ui->preivewpane->show();
        ui->varsetpane->show();
        ui->previewboot->show();
        ui->varsetgrpbtn->show();
        ui->sperator->show();
        ui->configurebodylabelsgrp->hide();
        break;

    case ModeType_Configure:
        m_pTouchPane->SetBackgroundTransparent(false);
        ui->configurebodylabelsgrp->show();
        ui->preivewpane->show();
        ui->varsetpane->show();
        ui->previewboot->show();
        ui->varsetgrpbtn->show();
        ui->sperator->show();
        ui->plorebodylabelsgrp->hide();
        break;

    default:

        break;
    }
}

void MainPane::OnDataTransfer(const QString& data)
{
    m_dgBinder.sendToServiceMessage(data.toLatin1());
}

void MainPane::OnMouseDataTransfer(const QString& data)
{
//    qDebug() << data.toLatin1();
    m_dgBinder.sendToServiceMessage(data.toLatin1(), false);
}

void MainPane::OnDataReceived(const QByteArray& dataArray)
{
//    const QString data = m_dgBinder.GetData();
    const QString data(dataArray);

    if(data.contains("VariantSets")) {
        QList<VarTree> list = CommonUtils::ParserVariantSets(data);
        ResourceManagerInst.SetVariantSets(list);
        m_pVariantPane->OnVariantSetsListUpdate();
    } else if(data.contains("LoadSceneReply")) {
        if(data.contains("LoadSceneReply{};"))
            return;

        int startIndex = QString("LoadSceneReply{").length() + data.indexOf("LoadSceneReply{");

        QString cpyData = data.trimmed();
        cpyData = cpyData.mid(startIndex, cpyData.length() - startIndex - 2);
        QStringList splitList = cpyData.split(",");
        QString name;
        bool result = false;
        foreach(QString item, splitList) {
            if(item.contains("name")) {
                name = item.split(":").at(1);
            }
            if(item.contains("result")) {
                result = (item.split(":").at(1) == "true");
            }
        }
        emit LoadSceneReplyUpdate(result, name);
    } else if(data.contains("RenderFinshed")) {
        m_pJobListDiag->ClearList();
        m_pRenderOnScreen->hide();
    } else {

    }
}

void MainPane::OnConnectStatusChanged(DeltaXBinder::ConnectState state)
{
    m_currentConnectState = state;
    if(m_currentConnectState != DeltaXBinder::ConnectState_Connect) return;
    // request list info
    const QString requestData("RequestVariantSet");
    OnDataTransfer(requestData);
}

void MainPane::onModeChanged(int index)
{
    if(index >= ModeType_Count) return;
    m_previewWinId = GetPreviewWinId();
    m_curModeType = (ModeType)index;
    ModeTypeChanged(m_curModeType);
    GeometryAdjust();
}

void MainPane::on_connectbtn_clicked()
{
    m_pVariantPane->ClearWidgets();
    emit PageJump(0);
}

void MainPane::on_addbtn_toggled(bool checked)
{
    if(checked) {
        m_pAddJobDiag->show();
        m_pJobListDiag->hide();
        if(ui->listbtn->isChecked())
            ui->listbtn->setChecked(false);
    } else {
        m_pAddJobDiag->hide();
    }
}

void MainPane::on_listbtn_toggled(bool checked)
{
    if(checked) {
        m_pAddJobDiag->hide();
        m_pJobListDiag->show();
        if(ui->addbtn->isChecked())
            ui->addbtn->setChecked(false);
    } else{
        m_pJobListDiag->hide();
    }
}

void MainPane::on_setbtn_toggled(bool checked)
{
    if(checked) {
        m_pSetHelpDiag->setVisible(true);
        if(ui->helpbtn->isChecked()) ui->helpbtn->setChecked(false);
        m_pSetHelpDiag->SetPaneVisible(true);
        m_pSetHelpDiag->HelpPaneVisible(false);
    } else {
        if(!ui->helpbtn->isChecked()) m_pSetHelpDiag->setVisible(false);
    }
}

void MainPane::on_helpbtn_toggled(bool checked)
{
    if(checked) {
        m_pSetHelpDiag->setVisible(true);
        if(ui->setbtn->isChecked()) ui->setbtn->setChecked(false);
        m_pSetHelpDiag->SetPaneVisible(false);
        m_pSetHelpDiag->HelpPaneVisible(true);
    } else {
        if(!ui->setbtn->isChecked()) m_pSetHelpDiag->setVisible(false);
    }
}

void MainPane::on_quitbtn_clicked()
{
    emit QuitRequest();
}

void MainPane::OnHelpButtonPressed(int index, bool flag)
{
    switch(index) {
    case 0:
        ui->grphelpbox->setVisible(flag);
        break;

    case 1:
        m_liveView = flag;
        ModeTypeChanged(m_curModeType);
        break;

    default:
        break;
    }
}

void MainPane::OnSetButtonPressed(int index)
{
    if( (index < 0) || (index >= LoginInfoList.count()) )
        return;

    LoginInfo info = LoginInfoList[index];
    HostInfo hostinfo;
    hostinfo.autoBinder = false;
    hostinfo.ipAddress = info.server;
    hostinfo.port = info.port;

    m_pVariantPane->ClearWidgets();
    m_pSetHelpDiag->hide();
    ui->setbtn->setChecked(false);

    VariantSetsBinderConnect(hostinfo);
}

void MainPane::OnServerListUpdate(const QStringList& strlist)
{
    m_pSetHelpDiag->UpdateServerList(strlist);
}

void MainPane::on_modelselectbtn_toggled(bool checked)
{
    m_pModelSelection->setVisible(checked);
}

void MainPane::SetPreviewTransparent(bool visible)
{
    switch(m_curModeType) {
    case ModeType_Preview:
    case ModeType_Plore:
        m_pTouchPane->SetBackgroundTransparent(visible);
        break;
    default:
        break;
    }
}

QByteArray MainPane::GetWaterMarkInfoMessage(const WaterMarkWidget::WaterMarkInfo& info, const QSize& size)
{
    QByteArray str("WaterMarkTask:");
    if(info.visible) {
        unsigned upos = 0x00;
        QPoint adjPos = info.pos;
        adjPos.setX((uint)(adjPos.x() * size.width() /  1920.0));
        adjPos.setY((uint)(adjPos.y() * size.height() / 1010.0));
        upos = adjPos.x();
        upos = upos << 16;
        upos |= adjPos.y();

        str.append(QString("Visible:%1;").arg(info.visible));
        str.append(QString("Font Family:%1;").arg(info.font.family()));
        str.append(QString("Font Size:%1;").arg(info.font.pixelSize()));
        str.append(QString("Font Bold:%1;").arg(info.font.bold()));
        str.append(QString("Font Italic:%1;").arg(info.font.italic()));
        str.append(QString("Color:%1;").arg(info.color.rgba()));
        str.append(QString("Position:%1;").arg(upos));
//        str.append(QString("Text:%1;").arg(info.text));
        str.append(QString("Text:%1;").arg(info.text).toLocal8Bit());
    } else {
        str.append(QString("Visible:%1;").arg(info.visible));
    }

    qDebug() << str;

    return str;
}
