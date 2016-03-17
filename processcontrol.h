#ifndef PROCESSCONTROL_H
#define PROCESSCONTROL_H

#include <QProcess>
#include <QString>
#include <QThread>

class ProcessControl :  public QObject
{
	Q_OBJECT

public:
	ProcessControl(QString &program, QObject *parent = 0);
	~ProcessControl();

private:
	QProcess*				m_process;

private slots:
	void processError(QProcess::ProcessError);
};

#endif // PROCESSCONTROL_H
