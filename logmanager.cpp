#include "logmanager.h"
#include <QCoreApplication>
#include <QDir>
#include <QDate>
#include <QTime>
#include <QDebug>

bool LogManager::isNewline = false;

LogManager::LogManager()
: QThread(NULL)
, m_pFile(NULL)
{
    init();

	start(NormalPriority);
}

LogManager::~LogManager()
{
    final();
}

void LogManager::init()
{
    MakeDir();
    FileOpen();
}

void LogManager::final()
{
    quit();
    if(!wait(1000)) {
        terminate();
        wait();
    }

    if(m_pFile) {
        if(m_stream.device() != m_pFile) {
            m_stream.setDevice(m_pFile);
        }
        m_stream << "\n";
        m_stream.flush();
        m_pFile->flush();
        m_pFile->close();
        m_pFile = NULL;
    }
}

bool LogManager::MakeDir()
{
    const QString folderName("Log");
    const QString currentPath = QDir::currentPath();
    m_currentDirPath = currentPath + "/" + folderName;
    bool rst = false;

    QDir d(folderName);
    if( !d.exists() )
        rst = d.mkdir(m_currentDirPath);

    return rst;
}

bool LogManager::FileOpen()
{
    QString fileName = QDate::currentDate().toString(Qt::ISODate) + QString(".txt");

    if(m_pFile == NULL)
        m_pFile = new QFile(m_currentDirPath + "/" + fileName);

    if(!m_pFile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        return false;

    return true;
}

void LogManager::LogDataClear()
{
    m_logContent.clear();
    m_logTime.clear();
    m_logDegree = "[Unknown] : ";
}

void LogManager::run()
{
    while(true) {
        if(!m_logOutput.isEmpty()) {
            if(m_pFile) {
                if(m_stream.device() != m_pFile)
                    m_stream.setDevice(m_pFile);
                m_stream << m_logOutput;
                m_logOutput.clear();
                start(IdlePriority);
            }
		}
		msleep(1000);
	}
}

void LogManager::Log(const QString& content, LogDegreeType degree)
{
    AddLogDegreeInfo(degree);
    AddLogContent(content);
    LogDataClear();

	this->start(NormalPriority);
}


LogManager& LogManager::operator << (const QString& t)
{
    AddLogContent(t);
    return *this;
}

LogManager& LogManager::operator << (const QByteArray& t)
{
    AddLogContent(t);
    return *this;
}

LogManager& LogManager::operator << (const char *c)
{
    const QByteArray t(c);
    AddLogContent(t);
    return *this;
}

LogManager& LogManager::operator << (const QList<QString>& t)
{
    foreach(QString item, t) {
        AddLogContent(item);
    }
    return *this;
}

LogManager& LogManager::operator << (const QList<QByteArray>& t)
{
    foreach(QByteArray item, t) {
        AddLogContent(item);
    }
    return *this;
}

LogManager& LogManager::operator << (const LogDegreeType t)
{
    AddLogDegreeInfo(t);
    return *this;
}

void LogManager::AddLogDegreeInfo(LogDegreeType t)
{
    switch(t) {
    case LogDegreeType_Normal:
        m_logDegree = "[Normal] : ";
        break;
    case LogDegreeType_Error:
        m_logDegree = "[Error] : ";
        break;
    case LogDegreeType_Warn:
        m_logDegree = "[Warn] : ";
        break;
    default:
        m_logDegree = "[Unknown] : ";
        break;
    }
}

void LogManager::AddLogContent(const QString& t)
{
    if(isNewline) {
        isNewline = false;
        m_logTime = QString("[") + QDateTime::currentDateTime().toString(Qt::ISODate) + QString("]");
        m_logOutput.append("\n");
        m_logOutput.append(m_logTime);
        m_logOutput.append(" ");
        m_logOutput.append(m_logDegree);
        m_logOutput.append(" ");
    }

    m_logContent = t;
    m_logOutput.append(m_logContent + " ");

    LogDataClear();
}

void LogManager::AddLogContent(const QByteArray& t)
{
    if(isNewline) {
        isNewline = false;
        m_logTime = QString("[") + QDateTime::currentDateTime().toString(Qt::ISODate) + QString("]");
        m_logOutput.append("\n");
        m_logOutput.append(m_logTime);
        m_logOutput.append(" ");
        m_logOutput.append(m_logDegree);
        m_logOutput.append(" ");
    }

    m_logContent = t;
    m_logOutput.append(m_logContent + " ");

    LogDataClear();
}
