#ifndef MAINPANE_H
#define MAINPANE_H

#include <QWidget>
#include <QButtonGroup>

#include "deltaxbinder.h"
#include "variantsetspane.h"
#include "touchpane.h"
#include "settinghelpdiag.h"
#include "addjobdiag.h"
#include "joblistdiag.h"
#include "modelselection.h"
#include "modelselectionex.h"
#include "renderonscreen.h"
#include "watermarkwidget.h"

#define DG_PORT                     (4500)

namespace Ui {
class MainPane;
}

class MainPane : public QWidget
{
    Q_OBJECT

public:
    enum ModeType { ModeType_Preview, ModeType_Plore, ModeType_Configure, ModeType_Count };

    explicit MainPane(QWidget *parent = 0);
    virtual ~MainPane();

    void CloseDgReflectWindow();

    virtual bool eventFilter(QObject *, QEvent *);

private:
    void init();
    void final();

    void GeometryAdjust();
    void PreviewWinGeometryAdjust();

    HWND GetPreviewWinId();
    void SocketClientSetting(const QString& ipAddress, uint port, bool autoConnnect = false);
    void ModeTypeChanged(ModeType);

    void SetPreviewTransparent(bool);
    QByteArray GetWaterMarkInfoMessage(const WaterMarkWidget::WaterMarkInfo&, const QSize&);

public slots:
    void VariantSetsBinderConnect(const HostInfo& host);
    void OnServerListUpdate(const QStringList&);

protected slots:
    virtual void moveEvent(QMoveEvent *);
    virtual void showEvent(QShowEvent *);
    virtual void hideEvent(QHideEvent *);
    virtual void closeEvent(QCloseEvent *);

private slots:
    void onModeChanged(int);
    void OnDataTransfer(const QString&);
    void OnMouseDataTransfer(const QString&);
    void OnDataReceived(const QByteArray&);
    void OnConnectStatusChanged(DeltaXBinder::ConnectState);

    //------------ set help diag
    void OnHelpButtonPressed(int, bool);
    void OnSetButtonPressed(int);

    void on_connectbtn_clicked();
    void on_addbtn_toggled(bool checked);
    void on_listbtn_toggled(bool checked);
    void on_setbtn_toggled(bool checked);
    void on_helpbtn_toggled(bool checked);
    void on_quitbtn_clicked();

    void on_modelselectbtn_toggled(bool checked);

    void OnAddNewRenderTask(const RenderData&);
    void OnStartTaskRender();

signals:
    void QuitRequest();
    void PageJump(int);
    void LoadSceneReplyUpdate(bool, const QString&);

private:
    Ui::MainPane      *ui;
    QButtonGroup*       m_pModeGrp;
    SettingHelpDiag*    m_pSetHelpDiag;
    AddJobDiag*         m_pAddJobDiag;
    JobListDiag*        m_pJobListDiag;
    HWND                m_previewWinId;
    DeltaXBinder        m_dgBinder;
//    DeltaXBinder        m_pbBinder;
    VariantSetsPane*    m_pVariantPane;
    TouchPane*          m_pTouchPane;
    ModelSelectionEx*     m_pModelSelection;
    DeltaXBinder::ConnectState     m_currentConnectState;
    ModeType            m_curModeType;
    bool                m_liveView;

    WaterMarkWidget*    m_pWaterMarkWidget;

    RenderOnScreen*     m_pRenderOnScreen;
};

#endif // MAINPANE_H
