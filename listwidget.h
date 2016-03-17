#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#include <QWidget>
#include <QList>
#include <QTime>
#include "widgetitem.h"
#include "uianimationmanager.h"

namespace Ui {
class ListWidget;
}

class ListWidget : public QWidget
{
    Q_OBJECT

public:
    enum FlipStatus{ FlipStatus_NULL , FlipStatus_Up, FlipStatus_Down };

    explicit ListWidget(QWidget *parent = 0);
    ~ListWidget();

    void SetItemSpace(const int space) { m_ItemSpace = space; }

    void AddWidgetItems(const QList<WidgetItem*>& list);
    void AddWidgetItem(WidgetItem* pItem);
    void Clear();

    void SetCursorEnable(const bool& enable) { m_cursorEnable = enable; }
    void SetCursorIndex(const int& index, bool visible = true);

    void SetRollColor(const QColor& color);

    void SetBackGroundColor(const QColor&);

protected:
    virtual void mousePressEvent(QMouseEvent*);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void wheelEvent(QWheelEvent *);
    virtual void resizeEvent(QResizeEvent *);

    virtual bool eventFilter(QObject *, QEvent *);

private:
    void init();
    void final();

    void InstallItemsFilter();
    void RemoveItemsFilter();

    void SetParent();
    void AdjustGeometry();
    void AdjustZOrder();

    void AdjustCursor();
    void AdjustRoll();

    bool RollChecked();
    void SetRollHeight(uint height);

private:
    Ui::ListWidget      *ui;

    int                 m_ItemSpace;

    int                 m_selectedIndex;

    bool                m_cursorEnable;
    bool                m_flip;
    bool                m_runed;
    FlipStatus          m_flipStatus;

    // flip animatoin
    uint                m_flipAnimInterval;
//    int                 m_flipGrabDistance;
    int                 m_flipGrabBeginY;
    QTime               m_flipTime;

    QWidget*            m_pWidgetListPane;
    QList<WidgetItem*>  m_list;

    UIAnimationManager  m_animManager;

signals:
    void SelecteChanged(int);

public slots:
    void OnSelectChanged(int);

private slots:
    void AnimationUpdate(const QVariant&);
    void AnimationFinished();
};

#endif // LISTWIDGET_H
