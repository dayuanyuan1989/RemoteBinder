// Copyright (C) 2013 Realtime Technology AG.
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

#include "UdpSender.h"
#include <QtNetwork/QtNetwork>

UdpSender::UdpSender() : m_sender(NULL) , m_port(9001)
{
    OnCreate();
}

UdpSender::~UdpSender()
{
    OnCleanUp();
}

void UdpSender::OnCreate()
{
	if(m_sender == NULL) {
		m_sender = new QUdpSocket(this);
	}
}

void UdpSender::OnCleanUp()
{
	if(m_sender) {
		delete m_sender;
		m_sender = NULL;
	}
}

void UdpSender::SendUdpMessage(QByteArray msg, uint port)
{
	if(port != 0) m_port = port;

    m_sender->writeDatagram(msg, msg.length(), QHostAddress::Broadcast, m_port);
}

void UdpSender::SendUdpMessage(QByteArray msg, const QString& ip, uint port)
{
    if(port != 0) m_port = port;

    m_sender->writeDatagram(msg, msg.length(), QHostAddress(ip), m_port);
}

void UdpSender::SendUdpMessage(QByteArray msg, const QHostAddress& addr, uint port)
{
    if(port != 0) m_port = port;

    m_sender->writeDatagram(msg, addr, m_port);
}

