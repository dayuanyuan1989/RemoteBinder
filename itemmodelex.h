#ifndef ITEM_MODELEX_H
#define ITEM_MODELEX_H

#include "ItemBase.h"
#include "scrolllabelex.h"
#include <QTimeline>
#include "scrolltext.h"

namespace Ui {
class ItemModelEx;
}

class ItemModelEx : public ItemBase
{
    Q_OBJECT

public:
    ItemModelEx(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ItemModelEx(const QPixmap& image, const QString& text, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~ItemModelEx();

    void SetStatus(Status status);
    Status GetStatus() { return m_status; }

    void ReloadResource() { m_bResourceInit = false; }
    void SetIconImage(const QString& image);
    void SetIconImage(const QPixmap& pixmap);
    void SetText(const QString& text);
    int GetProgressBarValue() const;
    void SetProgressBar(int value);
    void SetProgressBarVisible(bool);
    void StartProgressBarFade();
    void SetButtonText(const QString& text);

    void SetDownloadWidgetVisible(bool);
    void SetDownloadStateWidgetText(const QString&);

// event
protected:
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);

private slots:
    void on_pushButton_clicked();
    void OnFrameChanged(int);

private:
    void OnInit();
    void OnFinaliaze();

private:
    Ui::ItemModelEx*      	ui;
    Status                  m_status;
    // resource load handle
    bool					m_bResourceInit;

    QTimeLine               m_timeline;
    QGraphicsOpacityEffect  m_graphicsOpacityEffect;

    ScrollLabelEx*          m_pScrollLabel;
 //   ScrollText*             m_pScrollLabel;

signals:
    void ButtonPressed();
};

#endif // ITEM_MODELEX_H
