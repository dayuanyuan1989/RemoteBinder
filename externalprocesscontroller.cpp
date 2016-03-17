#include "externalprocesscontroller.h"
#include "logmanager.h"

ExternalProcessController::ExternalProcessController(QObject* parent)
    : QObject(parent)
    , m_process(NULL)
{
    m_process = new QProcess(this);

    connect(m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(OnProcessError(QProcess::ProcessError)));
    connect(m_process, SIGNAL(error(QProcess::ProcessError)), this, SIGNAL(Error(QProcess::ProcessError)));
}

ExternalProcessController::~ExternalProcessController()
{
    disconnect(m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(OnProcessError(QProcess::ProcessError)));
    disconnect(m_process, SIGNAL(error(QProcess::ProcessError)), this, SIGNAL(Error(QProcess::ProcessError)));

    m_process->terminate();
    m_process->waitForFinished();
}

void ExternalProcessController::Start(const QString& command)
{
    if(m_process) {
        LogManagerInst << LogManager::LogDegreeType_Normal << "Start External Process :" << command;
        m_process->start(command, QStringList(command));
    }
}

void ExternalProcessController::OnProcessError(QProcess::ProcessError error)
{
    QString errorStr;

    switch(error)
    {
    case QProcess::FailedToStart:
        errorStr = "FailedToStart";
        break;
    case QProcess::Crashed:
        errorStr = "Crashed";
        break;
    case QProcess::Timedout:
        errorStr = "FailedToStart";
        break;
    case QProcess::WriteError:
        errorStr = "Timedout";
        break;
    case QProcess::ReadError:
        errorStr = "ReadError";
        break;
    case QProcess::UnknownError:
        errorStr = "UnknownError";
        break;
    default:
        errorStr = "default";
        break;
    }

    LogManagerInst << LogManager::LogDegreeType_Warn << errorStr;
}
