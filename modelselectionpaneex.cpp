#include "modelselectionpaneex.h"
#include "ui_modelselectionpaneex.h"
#include "logmanager.h"
#include "itemmodel.h"

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QPainterPath>
#include <QMouseEvent>
#include <QTouchEvent>

#include <QDebug>

ModelSelectionPaneEx::ModelSelectionPaneEx(QWidget *parent)
:PaneBase(parent)
,ui(new Ui::ModelSelectionPaneEx)
,m_elementSize(QSize(240, 231))
,m_elementCount(0)
,m_curPaneIndex(0)
,m_paneIndexCount(0)
,m_bResourceInit(false)
,m_animation(NULL, "pos")
,m_signalMap(this)
{
    ui->setupUi(this);

    // single touch can't be check out.
    setAttribute(Qt::WA_AcceptTouchEvents);

    ui->itemsetpane->installEventFilter(this);
    connect(&m_animation, SIGNAL(finished()), this, SLOT(OnAnimationFinshed()));

    paintEvent(NULL);
}

ModelSelectionPaneEx::~ModelSelectionPaneEx()
{
    ui->itemsetpane->removeEventFilter(this);
    disconnect(&m_animation, SIGNAL(finished()), this, SLOT(OnAnimationFinshed()));
    FinalSignals();

    delete ui;
}

void ModelSelectionPaneEx::ReSize()
{
    const int elew = m_elementSize.width();
    const int eleh = m_elementSize.height();

    int tmpCount = m_elementCount;

    m_paneIndexCount = 0;
    if(tmpCount > 4) {
        m_paneIndexCount = tmpCount / 4;
        if(tmpCount % 4 != 0)
            m_paneIndexCount += 1;
    }

    const int w = m_paneIndexCount * (3 * ITEM_HORIZONTAL_SPACE + 2 * elew);
    const int h = ITEM_VERTICAL_SPACE + 2 * eleh;

    m_itemSetPaneSize = QSize(w, h);
    ui->itemsetpane->setGeometry(0, ui->itemsetpane->y(), w, h);
}

void ModelSelectionPaneEx::AddItems(QList<ItemBase*> btnList)
{
    const int elew = m_elementSize.width();
    const int eleh = m_elementSize.height();

    int x = 0;
    int y = 0;

    RemoveElements();

    m_elementList = btnList;
    m_elementCount = m_elementList.count();

    InitSignals();
    ReSize();
    PaneIndexDraw();

    // others
    for (int i = 0; i < m_elementList.count(); i++) {
        if(m_elementList[i] == NULL) return;

        switch(i%4) {
        case 0:
            x = ITEM_HORIZONTAL_SPACE + i/4*width();
            y = 0;
            break;
        case 1:
            x = ITEM_HORIZONTAL_SPACE*2 + elew + i/4*width();
            y = 0;
            break;
        case 2:
            x = ITEM_HORIZONTAL_SPACE + i/4*width();
            y = eleh + ITEM_VERTICAL_SPACE;
            break;
        case 3:
            x = ITEM_HORIZONTAL_SPACE*2 + elew + i/4*width();
            y = eleh + ITEM_VERTICAL_SPACE;
            break;
        default:
            break;
        }

        m_elementList[i]->setParent(ui->itemsetpane);
        m_elementList[i]->setGeometry(x, y, elew, eleh);
        m_elementList[i]->show();
    }
}

void ModelSelectionPaneEx::mousePressEvent(QMouseEvent*mouseEvent)
{
    m_time.start();
    m_lastX = mouseEvent->x();
    m_touchBeginLastX = mouseEvent->x();
}

void ModelSelectionPaneEx::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    int deltaX = mouseEvent->x() - m_lastX;
    m_lastX = mouseEvent->x();

    QPoint targetPos = ui->itemsetpane->pos()+QPoint(deltaX, 0);

    if(targetPos.x() > width()/4) {
        targetPos.setX(width()/4);
    } else if(targetPos.x() < width() - m_itemSetPaneSize.width() - width()/4) {
        targetPos.setX(width() - m_itemSetPaneSize.width() - width()/4);
    } else {

    }
    ui->itemsetpane->move(targetPos);
}

void ModelSelectionPaneEx::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
    int elapseTime = m_time.elapsed();
    float acceleration = (mouseEvent->x() - m_touchBeginLastX) / (float)elapseTime;

    bool flip = false;
    FlipDirection direction = acceleration > 0 ? FlipDirection_Left : FlipDirection_Right;
    if( qAbs(acceleration) > 1 || qAbs(mouseEvent->x() - m_touchBeginLastX) > width()/2 ) {
        flip = true;
        if( ((FlipDirection_Left == direction) && (m_curPaneIndex == 0))
                || ((FlipDirection_Right == direction) && (m_curPaneIndex == m_paneIndexCount - 1)) ) {
            flip = false;
        }
    }

    if(flip) {
        FlipAnimationStart(direction);
    } else {
        ResetPostion();
    }
}


void ModelSelectionPaneEx::paintEvent(QPaintEvent *)
{
    if(m_bResourceInit == false)
    {
        m_bResourceInit = true;
        PaneIndexResourceInit();
    }
}

void ModelSelectionPaneEx::PaneIndexResourceInit()
{
    QPixmap unselectedpix(INDEX_PIXMAP_RADIUS*2, INDEX_PIXMAP_RADIUS*2);
    QPixmap selectedpix(INDEX_PIXMAP_RADIUS*2, INDEX_PIXMAP_RADIUS*2);

    unselectedpix.fill(Qt::transparent);
    selectedpix.fill(Qt::transparent);

    QPainter painter;
    QBrush brush(QColor(234, 234, 234));

    painter.begin(&unselectedpix);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::darkGray);
    painter.setBrush(brush);
    painter.drawEllipse(QPoint(INDEX_PIXMAP_RADIUS, INDEX_PIXMAP_RADIUS), INDEX_UNSELECTION_RADIUS, INDEX_UNSELECTION_RADIUS);
    painter.end();
    m_unselectedPix = unselectedpix;

    painter.begin(&selectedpix);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::darkGray);
    painter.setBrush(brush);
    painter.drawEllipse(QPoint(INDEX_PIXMAP_RADIUS, INDEX_PIXMAP_RADIUS), INDEX_SELECTION_RADIUS, INDEX_SELECTION_RADIUS);
    painter.end();
    m_selectedPix = selectedpix;
}

bool ModelSelectionPaneEx::PaneIndexDraw()
{
    if( (m_curPaneIndex < 0) || (OnAnimation()) )
        return false;

    if(m_paneIndexCount > 0) {
        int indexPaneW = INDEX_ITEM_SPACE*(m_paneIndexCount+2);
        int indexPaneH = INDEX_PANE_HEIGHT;
        int indexPaneX = (width() - indexPaneW)/2;
        int indexPaneY = ui->indexwidget->y();

        ui->indexwidget->setVisible(true);
        ui->indexwidget->setGeometry(indexPaneX, indexPaneY, indexPaneW, indexPaneH);

        // new index labels
        if(m_indexLabelList.count() < m_paneIndexCount) {
            int mallocLabelCount = m_paneIndexCount - m_indexLabelList.count();
            for(int i = 0; i < mallocLabelCount; i++) {
                QLabel* pIndexLabel = new QLabel(ui->indexwidget);
                pIndexLabel->resize(INDEX_PIXMAP_RADIUS*2, INDEX_PIXMAP_RADIUS*2);
                m_indexLabelList.append(pIndexLabel);
            }
        }

        int x = 0;
        int y = 0;
        for(int i = 0; i < m_indexLabelList.count(); i++) {
            if(i < m_paneIndexCount) {
                m_indexLabelList[i]->setVisible(true);
                m_indexLabelList[i]->setPixmap(i == m_curPaneIndex ? m_selectedPix : m_unselectedPix);
                x = INDEX_ITEM_SPACE*(i+1) - INDEX_PIXMAP_RADIUS;
                y = (INDEX_PANE_HEIGHT - INDEX_PIXMAP_RADIUS*2) / 2;
                m_indexLabelList[i]->move(x, y);
            } else {
                m_indexLabelList[i]->setVisible(false);
            }
        }
    } else {
        ui->indexwidget->setVisible(false);
    }

    return true;
}

void ModelSelectionPaneEx::RemoveElements()
{
    FinalSignals();

    foreach(ItemBase* item, m_elementList) {
        delete item;
    }
    m_elementList.clear();
    m_elementCount = 0;
    m_curPaneIndex = 0;
    m_paneIndexCount = 0;
    PaneIndexDraw();
}

bool ModelSelectionPaneEx::OnAnimation()
{
    return (m_animation.state() == QAbstractAnimation::Running);
}

void ModelSelectionPaneEx::FlipAnimationStart(FlipDirection direction)
{
    if(FlipDirection_NULL == direction)
        return;

    if(OnAnimation()) {
        m_animation.setCurrentTime(300);
        m_animation.stop();
    }

    if(FlipDirection_Left == direction) {
        m_curPaneIndex--;
    } else {
        m_curPaneIndex++;
    }

    int source_x = ui->itemsetpane->x();
    int source_y = ui->itemsetpane->y();
    int target_x = m_curPaneIndex * -width();
    int target_y = source_y;

    m_animation.setStartValue(QPoint(source_x, source_y));
    m_animation.setEndValue(QPoint(target_x,target_y));
    m_animation.setTargetObject(ui->itemsetpane);
    m_animation.setDuration(300);
    m_animation.start();
}

void ModelSelectionPaneEx::ResetPostion(bool hasAnimation)
{
    QPoint targetPos(-width() * m_curPaneIndex, ui->itemsetpane->y());

    if(hasAnimation) {
        m_animation.setStartValue(ui->itemsetpane->pos());
        m_animation.setEndValue(targetPos);
        m_animation.setTargetObject(ui->itemsetpane);
        m_animation.setDuration(300);
        m_animation.start();
    } else {
        ui->itemsetpane->move(targetPos);
    }
}

void ModelSelectionPaneEx::OnAnimationFinshed()
{
    PaneIndexDraw();
}

void ModelSelectionPaneEx::SetProgressBarByIndex(int index, int value)
{
    if(index >= m_elementList.count())
        return;

    ItemModel* item = static_cast<ItemModel*>(m_elementList[index]);
    item->SetProgressBar(value);
}

void ModelSelectionPaneEx::InitSignals()
{
    int i = 0;
    for(i = 0; i < m_elementList.count(); i++) {
        ItemModel* pModel = static_cast<ItemModel*>(m_elementList[i]);
        connect(pModel, SIGNAL(ButtonPressed()), &m_signalMap, SLOT(map()));
        m_signalMap.setMapping(pModel, i);
    }
    connect(&m_signalMap, SIGNAL(mapped(int)), this, SLOT(OnElementButtonPressed(int)));
}

void ModelSelectionPaneEx::FinalSignals()
{
    int i = 0;
    for(i = 0; i < m_elementList.count(); i++) {
        ItemModel* pModel = static_cast<ItemModel*>(m_elementList[i]);
        disconnect(pModel, SIGNAL(ButtonPressed()), &m_signalMap, SLOT(map()));
    }
    disconnect(&m_signalMap, SIGNAL(mapped(int)), this, SLOT(OnElementButtonPressed(int)));
}

void ModelSelectionPaneEx::OnElementButtonPressed(int index)
{
    emit ButtonPressed(index);
}

