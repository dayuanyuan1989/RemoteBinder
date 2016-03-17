#ifndef MODELSELECTIONEX_H
#define MODELSELECTIONEX_H

#include <QWidget>
#include <QList>
#include <QSignalMapper>
#include <QScrollArea>

#include "deltaxbinder.h"
#include "itemmodelex.h"
#include "uianimationmanager.h"

namespace Ui {
class ModelSelectionEx;
}

class ModelSelectionEx : public QWidget
{
    Q_OBJECT

    enum ModelState
    {
        ModelState_Invalid,
        ModelState_Downloading,
        ModelState_Downloaded,
        ModelState_Opening,
        ModelState_Opened,
        ModelState_Count
    };

    typedef struct {
        ModelState download;
        QString id;
        QString name;
        QString modelname;
        int     thumbnailSize;
        QByteArray  thumbnail;
    } ModelInfo, *P2ModelInfo;

    enum SetId { SetId_Invalid, SetId_Update, SetId_Add, SetId_Modify, SetId_Delete, SetId_Count };

public:
    explicit ModelSelectionEx(QWidget *parent = 0);
    ~ModelSelectionEx();

    virtual bool eventFilter(QObject *, QEvent *);

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

    void HandleTestLoginResult(bool ret);

    void SetModels(QList<ItemModelEx*> list);

    void InitItemsSignals();
    void FinalItemsSignals();

private:
    Ui::ModelSelectionEx *ui;
    QWidget             m_itemsPane;
    QList<ItemModelEx*>   m_itemModelList;

    DeltaXBinder        m_pbBinder;
    DeltaXBinder::ConnectState  m_currentConnectState;

    QList<ModelInfo>    m_modelList;
    int                 m_curRequesetThumbnailIndex;
    int                 m_curThumbnailSize;
    QList<QString>      m_downloadIdList;

    SetId               m_curSetId;
    int                 m_curInfoIndex;

    bool                m_testing;

    QSignalMapper       m_signalMap;

    QScrollArea*        pScrollArea;

    UIAnimationManager  m_listAnimationManager;

public slots:
    void OnLoadSceneReplyUpdate(bool, const QString&);

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
    void OnItemButtonPressed(int);

    void OnSliderValueChanged(int);
    void OnSliderPressed();
};

#endif // MODELSELECTIONEX_H
