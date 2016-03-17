#include "UdpReceiver.h"
#include <QtNetwork/QtNetwork>
#include <QMessageBox>
#include <QByteArray>


UdpReceiver::UdpReceiver() : m_port(9001)
{
    m_receiver = new QUdpSocket(this);
    connect(m_receiver, SIGNAL(readyRead()), this, SLOT(dataReceived()));

    if (!m_receiver->bind(m_port, QUdpSocket::ShareAddress)) {
        m_data.append("Bind error");
        qDebug() << ("UdpReceiver : Bind error");
        return;
    }
}

UdpReceiver::~UdpReceiver()
{
    disconnect(m_receiver, SIGNAL(readyRead()), this, SLOT(dataReceived()));
}

// 处理等待的数据报
void UdpReceiver::dataReceived()
{
    // 拥有等待的数据报
    while(m_receiver->hasPendingDatagrams()) {
        QByteArray datagram;
        QHostAddress sender;   //客户端sender.toString()IP地址
        // 让datagram的大小为等待处理的数据报的大小，这样才能接收到完整的数据
        datagram.resize(m_receiver->pendingDatagramSize());
        // 接收数据报，将其存放到datagram中
        m_receiver->readDatagram(datagram.data(), datagram.size(), &sender, &m_port);
        m_data.append(datagram);
        emit newData();
    }
}
