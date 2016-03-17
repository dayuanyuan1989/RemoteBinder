#ifndef ITEM_MODEL_H
#define ITEM_MODEL_H

#include "ItemBase.h"
#include <QTimeline>

namespace Ui {
class ItemModel;
}

class ItemModel : public ItemBase
{
	Q_OBJECT

public:
    ItemModel(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ItemModel(const QPixmap& image, const QString& text, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~ItemModel();

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
    Ui::ItemModel*      	ui;
    Status                  m_status;
	// resource load handle
	bool					m_bResourceInit;

    QTimeLine               m_timeline;
    QGraphicsOpacityEffect  m_graphicsOpacityEffect;

signals:
    void ButtonPressed();
};

#endif // ITEM_MODEL_H
