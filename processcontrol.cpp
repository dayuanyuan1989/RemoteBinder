#include "processcontrol.h"
#include <QMessageBox>

ProcessControl::ProcessControl(QString &program, QObject *parent)
: QObject(parent)
{
	m_process = new QProcess(this);

	connect(m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError (QProcess::ProcessError)));	
	m_process->start(program);
}

ProcessControl::~ProcessControl()
{
	disconnect(m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError (QProcess::ProcessError)));	

	m_process->terminate();
	m_process->waitForFinished();
}

void ProcessControl::processError(QProcess::ProcessError error)
{
	switch(error)
	{
	case QProcess::FailedToStart:
		QMessageBox::information(0,"FailedToStart","FailedToStart");
		break;
	case QProcess::Crashed:
		QMessageBox::information(0,"Crashed","Crashed");
		break;
	case QProcess::Timedout:
		QMessageBox::information(0,"FailedToStart","FailedToStart");
		break;
	case QProcess::WriteError:
		QMessageBox::information(0,"Timedout","Timedout");
		break;
	case QProcess::ReadError:
		QMessageBox::information(0,"ReadError","ReadError");
		break;
	case QProcess::UnknownError:
		QMessageBox::information(0,"UnknownError","UnknownError");
		break;
	default:
		QMessageBox::information(0,"default","default");
		break;
	}
}
