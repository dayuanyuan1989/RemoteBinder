#ifndef DELTAXBINDER_H
#define DELTAXBINDER_H

#include <QObject>

#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>

#include <QByteArray>

typedef struct{bool autoBinder; QString ipAddress; uint port;} HostInfo;

class DeltaXBinder : public QObject
{
    Q_OBJECT

public:
    enum ConnectState{ ConnectState_Disconnect=0, ConnectState_Connect, ConnectState_Error, ConnectState_Invalid };

    DeltaXBinder(uint port = 4500);
    ~DeltaXBinder();

    bool sendToServiceMessage(const QByteArray& str, bool isBlock = true);

    void Connect();
    void DisConnect();

    void SetBinderAuto(bool var) { m_BinderAuto = var; }
    void SetBinderAddress(const QString& addr) { m_BinderAddress = addr; }
    QString GetBinderAddress() const { return m_BinderAddress; }
    void SetPortValue(const uint port) { m_port = port; }
    uint GetPort() const { return m_port; }

    QHostAddress getLocalAddress();
//    QString GetData() { QString cpy(m_readData); m_readData.clear(); return cpy; }

private:
    DeltaXBinder(const DeltaXBinder &);
    DeltaXBinder & operator = (const DeltaXBinder &);

    void serviceToConnect();
    void serviceToConnectRetry(const QByteArray& str);
    void serviceToDisConnect();

Q_SIGNALS:
    void BinderStatusChanged(DeltaXBinder::ConnectState);
    void DataReceived(const QByteArray&);

private slots:
    // socket event handle
    void onConnected();
    void onDisconnected();
    void onDataReceived();
    void onDisplayError(QAbstractSocket::SocketError);

private:
    QTcpSocket*  m_pSocket;
    quint16      m_port;

    bool         m_BinderAuto;
    QString      m_BinderAddress;

//    QString      m_readData;
};

#endif // DELTAXBINDER_H
