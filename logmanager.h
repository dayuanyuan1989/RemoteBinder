#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <QThread>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QByteArray>

class LogManager : public QThread
{
	Q_OBJECT
public:
    enum LogDegreeType { LogDegreeType_Invalid = -1, LogDegreeType_Normal = 0, LogDegreeType_Warn, LogDegreeType_Error, LogDegreeType_Unknown, LogDegreeType_Count };

    static LogManager& GetInstance()
	{
        isNewline = true;

        static LogManager instance;
		return instance;  
	}

    void Log(const QString&, LogDegreeType degree = LogDegreeType_Unknown);

    LogManager& operator << (const QString& t);
    LogManager& operator << (const QByteArray& t);
    LogManager& operator << (const char *c);
    LogManager& operator << (const QList<QString>& t);
    LogManager& operator << (const QList<QByteArray>& t);
    LogManager& operator << (const LogDegreeType t);

protected:
	virtual void run();

private:
    LogManager();
    LogManager(const LogManager&);
    LogManager& operator= (const LogManager&);
	virtual ~LogManager();

    void init();
    void final();

    bool MakeDir();
    bool FileOpen();

    void LogDataClear();

    void AddLogDegreeInfo(LogDegreeType t);
    void AddLogContent(const QString& t);
    void AddLogContent(const QByteArray& t);

private:
    QFile*          m_pFile;
    QTextStream     m_stream;

    QString         m_currentDirPath;

    QByteArray      m_logOutput;
    QString         m_logContent;
    QString         m_logTime;
    QString         m_logDegree;

    static bool     isNewline;
};

#define LogManagerInst  LogManager::GetInstance()

#endif // LOG_MANAGER_H
