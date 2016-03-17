#include "pagemanager.h"
#include "resourcemanager.h"

#include "UdpSender.h"
#include "logmanager.h"

#include <QFile>
#include <QDataStream>
#include <QDir>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QApplication>
#include <QDesktopWidget>

#define PREVIEW_PORT    (9001)

PageManager::PageManager(QObject* parent)
    : QObject(parent)
    , m_curPageId(PageInvalid)
    , m_pBaseWidget(NULL)
{
    LoadUserInfo();

    init();
}

PageManager::~PageManager()
{
 //   final();
    SaveUserInfo();
}

void PageManager::init()
{
    m_pBaseWidget = new QWidget();
    m_pBaseWidget->resize(1920, 1080);
    m_pBaseWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    m_pBaseWidget->setAttribute(Qt::WA_TranslucentBackground);

    QDesktopWidget desktopWidget;
    int screenCount = desktopWidget.screenCount();
    if( (screenCount > 1) && (ResourceManagerInst.isShowOnExtensionScreen()) ) {
        m_pBaseWidget->move( desktopWidget.screenGeometry(1).topLeft() );
    }

    m_pageList.append(new LoginPane);
    m_pageList.append(new MainPane);

    foreach(QWidget* pPage, m_pageList) {
        pPage->setParent(m_pBaseWidget);
        pPage->hide();
        connect(pPage, SIGNAL(PageJump(int)), this, SLOT(OnPageJump(int)));
        connect(pPage, SIGNAL(QuitRequest()), this, SLOT(OnQuit()));
    }

    connect(m_pageList[0], SIGNAL(ConnectRequest(QString, uint)), this, SLOT(OnConnectHandle(QString, uint)));
    connect(m_pageList[0], SIGNAL(ServerListUpdate(QStringList)), m_pageList[1], SLOT(OnServerListUpdate(QStringList)));

    m_pBaseWidget->show();
}

void PageManager::final()
{
    disconnect(m_pageList[0], SIGNAL(ConnectRequest(QString, uint)), this, SLOT(OnConnectHandle(QString, uint)));
    disconnect(m_pageList[0], SIGNAL(ServerListUpdate(QStringList)), m_pageList[1], SLOT(OnServerListUpdate(QStringList)));
    reinterpret_cast<MainPane*>(m_pageList[1])->CloseDgReflectWindow();
    foreach(QWidget* pPage, m_pageList) {
        disconnect(pPage, SIGNAL(PageJump(int)), this, SLOT(OnPageJump(int)));
        disconnect(pPage, SIGNAL(QuitRequest()), this, SLOT(OnQuit()));
        /* watch out:
            当一个QObject正在接受事件队列时如果中途被你DELETE掉了，
            就是出现问题了，所以Qt中建议大家不要直接DELETE掉一个QObject，
            如果一定要这样做，要使用QObject的deleteLater()函数，
            它会让所有事件都发送完一切处理好后马上清除这片内存，
            而且就算调用多次的deletelater也不会有问题。
        */
 //       pPage->deleteLater();
    }

    m_pageList.clear();
    m_curPageId = PageInvalid;
}

void PageManager::OnPageJump(int idx)
{
    PageId id = (PageId)idx;
    if((id >= PageId_Count) || (id == m_curPageId)) return;
    if((m_pageList[id] == NULL) || m_pageList[m_curPageId] == NULL) return;

    m_pageList[m_curPageId]->hide();
    m_pageList[id]->show();
    m_curPageId = id;
}

void PageManager::OnConnectHandle(const QString& ipText, uint port)
{
    HostInfo info;
    info.ipAddress = ipText;
 //   info.port = DG_PORT;
    info.port = port;
    info.autoBinder = false;
    reinterpret_cast<MainPane*>(m_pageList[1])->VariantSetsBinderConnect(info);

    UdpSenderInst.SendUdpMessage(info.ipAddress.toLatin1(), QHostAddress::LocalHost ,PREVIEW_PORT);
    LogManagerInst << LogManager::LogDegreeType_Normal << "Connect to C-Plore IP :" << info.ipAddress.toLatin1();
//    UdpSenderInst.SendUdpMessage(info.ipAddress.toLatin1(), "10.90.111.102" ,PREVIEW_PORT);
}

void PageManager::ShowPageInit(PageId id)
{
    m_curPageId = id;
    m_pageList[m_curPageId]->show();
}

void PageManager::OnQuit()
{
    final();
    m_pBaseWidget->close();
}

void PageManager::SaveUserInfo()
{
    QJsonArray loginInfoArray;
    foreach(LoginInfo info, LoginInfoList) {
        QJsonObject loginInfoObj;
        loginInfoObj.insert("titile", info.title);
        loginInfoObj.insert("server", info.server);
        loginInfoObj.insert("port", (int)info.port);
        loginInfoArray.append(loginInfoObj);
    }

    QJsonArray userInfoArray;
    foreach(PbInfo info , PbInfoList) {
        QJsonObject pbInfoObj;
        pbInfoObj.insert("title", info.title);
        pbInfoObj.insert("address", info.address);
        pbInfoObj.insert("port", (int)info.port);
        pbInfoObj.insert("user", info.user);
        pbInfoObj.insert("password", info.password);
        userInfoArray.append(pbInfoObj);
    }

    QJsonObject info;
    info.insert("LoginInfo", loginInfoArray);
    info.insert("PbInfo", userInfoArray);

    QByteArray infoBytes = QJsonDocument(info).toJson();
    QByteArray convertData;

    int i = 0;
    for(i = 0; i < infoBytes.size(); i++) {
        convertData.append( (infoBytes.at(i)+i)%0xff );
    }

    QFile file(QDir::currentPath() + "/info.dat");
    file.open(QIODevice::WriteOnly);
    QDataStream steam(&file);
    steam << convertData;
    file.close();
}

void PageManager::LoadUserInfo()
{
    QFile file(QDir::currentPath() + "/info.dat");

    if( !file.open(QIODevice::ReadOnly))
        return;


    QDataStream steam(&file);
    QByteArray infoData;
    steam >> infoData;
    file.close();

    QByteArray convertData;
    int i = 0;
    for(i = 0; i < infoData.size(); i++) {
        uchar ch = infoData.at(i);
        convertData.append( (ch + (i/0xff+1)*0xff-i) % 0xff );
    }

    QJsonParseError error;
    QJsonDocument jdocument = QJsonDocument::fromJson(convertData, &error);
    if(error.error == QJsonParseError::NoError) {
        if(jdocument.isObject()) {
            QJsonObject jdocumentobj = jdocument.object();
            QJsonArray jLoginJsonArray = jdocumentobj.value("LoginInfo").toArray();
            int j = 0;
            LoginInfo info;
            QJsonObject jobj;
            for(j = 0;  j < jLoginJsonArray.count(); j++) {
                jobj = jLoginJsonArray.at(j).toObject();
                info.title = jobj.value("titile").toString();
                info.server = jobj.value("server").toString();
                info.port = jobj.value("port").toInt();

                LoginInfoList.append(info);
            }

            PbInfo pbInfo;
            QJsonArray jPbJsonArray = jdocumentobj.value("PbInfo").toArray();
            for(j = 0;  j < jPbJsonArray.count(); j++) {
                jobj = jPbJsonArray.at(j).toObject();
                pbInfo.title = jobj.value("title").toString();
                pbInfo.address = jobj.value("address").toString();
                pbInfo.port = jobj.value("port").toInt();
                pbInfo.user = jobj.value("user").toString();
                pbInfo.password = jobj.value("password").toString();

                PbInfoList.append(pbInfo);
            }
        }
    }
}
