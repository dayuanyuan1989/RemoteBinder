#include "deltaxbinder.h"
#include <QNetworkInterface>
#include <QHostInfo>
#include <QCoreApplication>
#include <UdpSender.h>

#include "logmanager.h"
#include <QDebug>

DeltaXBinder::DeltaXBinder(uint port)
:m_pSocket(NULL), m_port(port), m_BinderAuto(false)
{

}

DeltaXBinder::~DeltaXBinder()
{
    serviceToDisConnect();
}

void DeltaXBinder::Connect()
{
    serviceToConnect();
}

void DeltaXBinder::DisConnect()
{
    serviceToDisConnect();
}

void DeltaXBinder::onConnected()
{
    qDebug() << QString("onConnected ");
    emit BinderStatusChanged(ConnectState_Connect);
}

void DeltaXBinder::onDisconnected()
{
    qDebug() << QString("onDisconnected ");

    emit BinderStatusChanged(ConnectState_Disconnect);

    if(NULL != m_pSocket) {
        m_pSocket->close();
        m_pSocket = NULL;
    }
}

void DeltaXBinder::onDataReceived()
{
    qDebug() << QString("onDataReceived ");

    static bool getHead = false;
    static int head = 0;

    while(m_pSocket->bytesAvailable() >= 4) {
        // get head info
        if(!getHead){
            QByteArray headStr = m_pSocket->read(4);
            head = headStr.toUInt(&getHead);
        }

        if(getHead) {
            if(m_pSocket->bytesAvailable() < head) {
                break;
            } else {
                QByteArray body = m_pSocket->read(head);
                emit DataReceived(body);
                getHead = false;
                head = 0;
            }
        }
    }

}

void DeltaXBinder::onDisplayError(QAbstractSocket::SocketError errID)
{
    LogManagerInst << LogManager::LogDegreeType_Normal << QString("onDisplayError ") + QString::number(errID);
    qDebug() << QString("onDisplayError ") + QString::number(errID);
    emit BinderStatusChanged(ConnectState_Error);

    switch(errID) 	{
    case QAbstractSocket::NetworkError:
    case QAbstractSocket::ConnectionRefusedError:
        serviceToDisConnect();
        break;
    default:
        break;
    }
}

QHostAddress DeltaXBinder::getLocalAddress()
{
    int index = 0;
    QList<QHostAddress> adds;
    QList<QHostAddress> localAddrs;

#define HOST_ADDRESS_FROM_HOST_INFO
#ifdef HOST_ADDRESS_FROM_HOST_INFO
    localAddrs = (QHostInfo::fromName(QHostInfo::localHostName())).addresses();
#else
    // error display when QNetworkInterface::allAddresses() to disconstruct
    qDebug() << "Get host address from QNetworkInterface::allAddresses(), when disconstruct, error display";
    localAddrs = QNetworkInterface::allAddresses();
#endif

    foreach(QHostAddress addr, localAddrs)
    {
        /* To pick up all ipv4 addr */
        if(QAbstractSocket::IPv4Protocol == addr.protocol())
        {
            adds.append(addr);
        }
    }

    if(adds.count() == 0){
        QHostAddress hostAddr;
        return hostAddr;
    }

    /* Count of ipv4 addrs is more than 1, to pick the second addr */
    if(adds.count() > 1){
        index = 1;
    }

    qDebug() << QString("----------ipv4 list----------");
    foreach(QHostAddress addr, adds)
    {
        QString addrStr = addr.toString();
        qDebug() <<  addrStr;
    }

    return adds[index];
}

void DeltaXBinder::serviceToConnect()
{
    QHostAddress toConnectedHost;
    m_pSocket = new QTcpSocket(this);
    m_pSocket->abort();

    QObject::connect(m_pSocket, SIGNAL(connected()), this, SLOT(onConnected()));
    QObject::connect(m_pSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    QObject::connect(m_pSocket, SIGNAL(readyRead()),this, SLOT(onDataReceived()));
    QObject::connect(m_pSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onDisplayError(QAbstractSocket::SocketError)));

    if(m_BinderAuto) {
        toConnectedHost = getLocalAddress();
        m_BinderAddress = toConnectedHost.toString();
        m_pSocket->connectToHost(toConnectedHost, m_port);
    }
    else {
        toConnectedHost.setAddress(m_BinderAddress);
        m_pSocket->connectToHost(toConnectedHost, m_port);
    }

    qDebug() << QString("----------used ipv4----------");
    qDebug() << QString("Connect Host is: ") + toConnectedHost.toString();
}

void DeltaXBinder::serviceToConnectRetry(const QByteArray& str)
{
    serviceToConnect();
    sendToServiceMessage(str);
}

void DeltaXBinder::serviceToDisConnect()
{
    if(m_pSocket != NULL) {
        QObject::disconnect(m_pSocket, SIGNAL(connected()), this, SLOT(onConnected()));
        QObject::disconnect(m_pSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
        QObject::disconnect(m_pSocket, SIGNAL(readyRead()),this, SLOT(onDataReceived()));
        QObject::disconnect(m_pSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onDisplayError(QAbstractSocket::SocketError)));

        m_pSocket->close();
        m_pSocket = NULL;
    }
}

bool DeltaXBinder::sendToServiceMessage(const QByteArray& str, bool isBlock)
{
    static QByteArray copy_str;
    static int counter = 0;

    if(m_pSocket == NULL) {
        qDebug() <<  QString("Socket is NULL");
        qDebug() << QString("Retry to connect to the Server");
        serviceToConnectRetry(str);
        return false;
    }

    if(copy_str == str) {
        counter++;
    } else {
        copy_str = str;
        counter = 0;
    }

    if((counter >= 3) && !str.contains("OPT")) {
        return false;
    } else
    {
        if(	m_pSocket->state() == QAbstractSocket::ConnectedState) {
            if(str.length() < 10000) {
                QByteArray headArray = QByteArray::number(str.length());
                int InsertZeroCount = 4 - headArray.length();
                while(InsertZeroCount) {
                    headArray.insert(0, QByteArray::number(0));
                    InsertZeroCount--;
                }
 //               qDebug() << "Head Length : " << headArray.length();
                QByteArray sendMsg;
                if(isBlock) {
                    sendMsg.append(headArray);
                    sendMsg.append(str);
                    m_pSocket->write(sendMsg);
                    m_pSocket->flush();
                    /*watch out:
                        Surprise, if no action -- not to run tcp socket, then run to
                        QCoreApplication::processEvents(), program cracked.
                    */
                    QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);
                } else {
                    sendMsg.append(str);
                    UdpSenderInst.SendUdpMessage(sendMsg, m_BinderAddress, 9002);
                }
  //              qDebug() << "Send Message" << sendMsg;

  //              QCoreApplication::processEvents(QEventLoop::WaitForMoreEvents);

                return true;
            }
            return false;
        } else {
            qDebug() << "Msg Send Failed, not connected";
            return false;
        }
    }
}
