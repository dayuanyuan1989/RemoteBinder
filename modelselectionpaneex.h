#ifndef MODELSELECTIONPANEEX_H
#define MODELSELECTIONPANEEX_H

#include "panebase.h"
#include "itembase.h"

#include <QPropertyAnimation>
#include <QSize>
#include <QList>
#include <QLabel>
#include <QTime>
#include <QSignalMapper>

#define INDEX_PANE_HEIGHT   (14)
#define INDEX_ITEM_SPACE    (17)
#define INDEX_PIXMAP_RADIUS (INDEX_SELECTION_RADIUS>INDEX_UNSELECTION_RADIUS?INDEX_SELECTION_RADIUS:INDEX_UNSELECTION_RADIUS)
#define INDEX_UNSELECTION_RADIUS  (5)
#define INDEX_SELECTION_RADIUS  (6)

#define ITEM_HORIZONTAL_SPACE   (10)
#define ITEM_VERTICAL_SPACE     (60)

namespace Ui {
class ModelSelectionPaneEx;
}

class ModelSelectionPaneEx : public PaneBase
{
    Q_OBJECT

public:
    enum FlipDirection{ FlipDirection_NULL , FlipDirection_Left, FlipDirection_Right };

    explicit ModelSelectionPaneEx(QWidget *parent = 0);
    ~ModelSelectionPaneEx();

    bool OnAnimation();

    void AddItems(QList<ItemBase*> btnList);
    void ClearItems() { m_elementList.clear(); }

    void ReloadResource() { m_bResourceInit = false; }
    QSize PaneFixedSize() const { return m_itemSetPaneSize; }

    void SetProgressBarByIndex(int index, int value);

    void RemoveElements();

protected:
    virtual void mousePressEvent(QMouseEvent*);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void paintEvent(QPaintEvent *);

private:
    void PaneIndexResourceInit();

    void ReSize();
    bool PaneIndexDraw();
    // flip buttons handle
    void FlipAnimationStart(FlipDirection direction);
    void ResetPostion(bool hasAnimation = true);

    void InitSignals();
    void FinalSignals();

    // unused, just for debug
    inline void SetElementSize(QSize size) { m_elementSize = size; ReSize(); }
    inline void SetElementCount(int count) { m_elementCount = count; ReSize(); }

private:
    Ui::ModelSelectionPaneEx *ui;
    QSize			m_elementSize;
    int				m_elementCount;
    QList<ItemBase*>	m_elementList;

    // pane size
    QSize			m_itemSetPaneSize;

    // pane index
    int				m_curPaneIndex;
    int				m_paneIndexCount;
    QList<QPointF>	m_indexPointList;
    QList<QLabel*>   m_indexLabelList;

    bool			m_bResourceInit;

    // animation
    QPropertyAnimation	m_animation;

    // index resource
    QPixmap         m_selectedPix;
    QPixmap         m_unselectedPix;

    QTime           m_time;
    int             m_lastX;
    int             m_touchBeginLastX;

    QSignalMapper   m_signalMap;

signals:
    void ButtonPressed(int);

private slots:
    void OnAnimationFinshed();
    void OnElementButtonPressed(int);
};

#endif // MODELSELECTIONPANEEX_H
