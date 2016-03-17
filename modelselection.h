#ifndef MODELSELECTION_H
#define MODELSELECTION_H

#include <QWidget>
#include <QList>

#include "modelselectionpaneex.h"
#include "deltaxbinder.h"
#include "itemmodel.h"

namespace Ui {
class ModelSelection;
}

class ModelSelection : public QWidget
{
    Q_OBJECT

    enum DownloadState { DownloadState_Invalid, DownloadState_Downloaded, DownloadState_Downloading, DownloadState_Count };

    typedef struct {
        DownloadState download;
        QString id;
        QString name;
        QString modelname;
        int     thumbnailSize;
        QByteArray  thumbnail;
    } ModelInfo, *P2ModelInfo;

    enum SetId { SetId_Invalid, SetId_Update, SetId_Add, SetId_Modify, SetId_Delete, SetId_Count };

public:
    explicit ModelSelection(QWidget *parent = 0);
    ~ModelSelection();

private:
    void init();
    void final();

    void Connect(const HostInfo&);
    void CurrentInfoConnect(int);
    void SetTestButtonBackOff();

    //--------------------------------------- Model List
    int GetIndexById(const QString&);
    void RequesetThumbnail();

    //--------------------------------------- Download Flag Manager
    void OnDownloadFinished(int);

    bool TryConvetPixmapFromData(QPixmap&, const QByteArray&);

    QByteArray GetCurrentLoginInfoData();

private:
    Ui::ModelSelection *ui;
    ModelSelectionPaneEx* m_pModelSelectionPane;
    QList<ItemModel*>   m_itemModelList;

    DeltaXBinder        m_pbBinder;
    DeltaXBinder::ConnectState  m_currentConnectState;

    QList<ModelInfo>    m_modelList;
    int                 m_curRequesetThumbnailIndex;
    int                 m_curThumbnailSize;
    QList<QString>      m_downloadIdList;

    SetId               m_curSetId;
//    QList<PbInfo>       m_pbInfoList;
    int                 m_curInfoIndex;

    bool                m_testing;


private slots:
    //--------------------------------------- Handle Buttons
    void on_refreshBtn_clicked();
    void on_addBtn_clicked();
    void on_editbtn_clicked();
    void on_deleteBtn_clicked();
    void on_testlogin_clicked();
    void on_submitBtn_clicked();
    void on_cancelBtn_clicked();
    void on_comboBox_activated(int index);

    //--------------------------------------- Handle PB signals
    void OnDataReceived(const QByteArray&);
    void OnConnectStatusChanged(DeltaXBinder::ConnectState);

    //--------------------------------------- Handle ModelSelectionPane
    void OnModelSelectionPaneItemPressed(int);
};

#endif // MODELSELECTION_H
