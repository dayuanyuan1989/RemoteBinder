#ifndef UDPRECEIVER_H
#define UDPRECEIVER_H

#include <QtCore/QObject>
#include <QString>

class QUdpSocket;  

class UdpReceiver : public QObject  
{  
	Q_OBJECT  

public:  
	static UdpReceiver& GetInstance() {
		static UdpReceiver instance;
		return instance;
	}

	inline QString Data() { QString cpy(m_data); m_data.clear(); return cpy; }
		
private:
	UdpReceiver(); 
	UdpReceiver(const UdpReceiver&);
	UdpReceiver& operator=(const UdpReceiver&);
	virtual ~UdpReceiver();  

private:  
	QUdpSocket*		m_receiver;  
	quint16			m_port;  

	QString			m_data;

signals:
	void newData();

private slots:  
	void dataReceived();
};  

#define UdpReceiverInstance UdpReceiver::GetInstance()

#endif // UDPRECEIVER_H
