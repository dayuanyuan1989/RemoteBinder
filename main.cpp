#include <QApplication>
#include <QImageReader>
#include <QDir>
#include <QLibrary>

#include <windows.h>

#include "resourcemanager.h"
#include "pagemanager.h"
#include "logmanager.h"
#include "externalprocesscontroller.h"

#include "modelselectionex.h"
#include "syscertification.h"
#include <openssl/applink.c>
#include <QMessageBox>

#include "watermarkwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QList<QByteArray> supportlist = QImageReader::supportedImageFormats();
    qDebug() << supportlist;
    LogManagerInst << LogManager::LogDegreeType_Normal << supportlist;

    SysCertification certificate;
    if(certificate.CheckSysInfo()) {
        ResourceManagerInst.FileDataInfoInit();
        ResourceManagerInst.ResourceInit();

        ExternalProcessController externalProcesser;
        externalProcesser.Start(QDir::currentPath() + "/CPlore.exe");

        PageManagerInst.ShowPageInit(PageManager::PageId_Begin);
    } else {
        if(QMessageBox::Ok == QMessageBox::information(NULL, "Warning", "Certificate Failed!")) {
            a.exit(-1);
            return -1;
        }
    }

    return a.exec();
}
