// Copyright (C) 2015 Realtime Technology AG.
// All rights reserved.
//
// This software and related documentation are proprietary to
// Realtime Technology AG.
//
// This software has been provided pursuant to a Software License Agreement
// containing restrictions on its use. This software contains valuable trade
// secrets and proprietary information of Realtime Technology AG and is
// protected by law. Except as provided for in the Software License Agreement or
// by mandatory law this software may not be copied, replicated, decompiled,
// reverse-engineered or reproduced in any form or medium. Further, it may not
// be disclosed, distributed or made available to third parties or used in any
// manner not provided for in the Software License Agreement without the prior
// written consent of Realtime Technology AG.
//
// This software is licensed on an "as is" basis without warranty regarding any
// result that may be obtained by using this software or fitness of this
// software for any particular purpose.

#ifndef UDP_SENDER_H
#define UDP_SENDER_H

#include <QtCore/QObject>
#include <QHostAddress>

class QUdpSocket;

class UdpSender : public QObject
{
	Q_OBJECT

public:
	static UdpSender& GetInstance() 
	{
		static UdpSender instance;
		return instance;
	}

	void SendUdpMessage(QByteArray msg, uint port = 0);
    void SendUdpMessage(QByteArray msg, const QString& ip, uint port = 0);
    void SendUdpMessage(QByteArray msg, const QHostAddress& addr, uint port = 0);

private:
	UdpSender();
	UdpSender(const UdpSender&);
	UdpSender& operator=(const UdpSender&);
	virtual ~UdpSender();

	void OnCreate();
	void OnCleanUp();

private:
	QUdpSocket*		m_sender;  
	uint			m_port;
};

#define UdpSenderInst   UdpSender::GetInstance()

#endif	// UDP_SENDER_H
