#ifndef EXTERNALPROCESSCONTROLLER_H
#define EXTERNALPROCESSCONTROLLER_H

#include <QProcess>
#include <QObject>

class ExternalProcessController : public QObject
{
    Q_OBJECT
public:
    ExternalProcessController(QObject* parent = 0);
    ~ExternalProcessController();

    void Start(const QString&);

private:
    QProcess*       m_process;

signals:
    Error(QProcess::ProcessError);

private slots:
    void OnProcessError(QProcess::ProcessError);
};

#endif // EXTERNALPROCESSCONTROLLER_H
