#include "modelselectionpane.h"
#include "ui_modelselectionpane.h"

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QPainterPath>
#include <QMouseEvent>
#include <QTouchEvent>

#include "logmanager.h"
#include <QDebug>

ModelSelectionPane::ModelSelectionPane(QWidget *parent, Qt::WindowFlags flags)
:PaneBase(parent, flags)
,ui(new Ui::ModelSelectionPane)
,m_elementSize(QSize(240, 231))
,m_elementCount(0)
,m_curPaneIndex(0)
,m_prePaneIndex(0)
,m_paneIndexCount(0)
,m_bResourceInit(false)
,m_animation(NULL, "pos")
{
    ui->setupUi(this);

    setAttribute(Qt::WA_AcceptTouchEvents);

    ui->itemsetpane->installEventFilter(this);
    connect(&m_animation, SIGNAL(finished()), this, SLOT(OnAnimationFinshed()));

    paintEvent(NULL);
}

ModelSelectionPane::~ModelSelectionPane()
{
    ui->itemsetpane->removeEventFilter(this);
    disconnect(&m_animation, SIGNAL(finished()), this, SLOT(OnAnimationFinshed()));
}

void ModelSelectionPane::ReSize()
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

void ModelSelectionPane::AddItems(QList<ItemBase*> btnList)
{
    const int elew = m_elementSize.width();
    const int eleh = m_elementSize.height();

    int x = 0;
    int y = 0;

    m_elementList.append(btnList);
    m_elementCount = m_elementList.count();

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
	}
}

void ModelSelectionPane::mousePressEvent(QMouseEvent*mouseEvent)
{
    m_time.start();
    m_lastX = mouseEvent->x();
    m_touchBeginLastX = mouseEvent->x();
}

void ModelSelectionPane::mouseMoveEvent(QMouseEvent *mouseEvent)
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

void ModelSelectionPane::mouseReleaseEvent(QMouseEvent *mouseEvent)
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


void ModelSelectionPane::paintEvent(QPaintEvent *)
{
    if(m_bResourceInit == false)
    {
        m_bResourceInit = true;
        PaneIndexResourceInit();
    }
}

void ModelSelectionPane::PaneIndexResourceInit()
{
    QPixmap unselectedpix(INDEX_PIXMAP_RADIUS*2, INDEX_PIXMAP_RADIUS*2);
    QPixmap selectedpix(INDEX_PIXMAP_RADIUS*2, INDEX_PIXMAP_RADIUS*2);

    unselectedpix.fill(Qt::transparent);
    selectedpix.fill(Qt::transparent);

    QPainter painter;
    QBrush brush(QColor(0xee, 0xee, 0xee, 0xff));

    painter.begin(&unselectedpix);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.drawEllipse(QPoint(INDEX_PIXMAP_RADIUS, INDEX_PIXMAP_RADIUS), INDEX_UNSELECTION_RADIUS, INDEX_UNSELECTION_RADIUS);
    painter.end();
    m_unselectedPix = unselectedpix;

    painter.begin(&selectedpix);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.drawEllipse(QPoint(INDEX_PIXMAP_RADIUS, INDEX_PIXMAP_RADIUS), INDEX_SELECTION_RADIUS, INDEX_SELECTION_RADIUS);
    painter.end();
    m_selectedPix = selectedpix;
}

bool ModelSelectionPane::PaneIndexDraw()
{
    if((m_curPaneIndex >= m_paneIndexCount) || (m_curPaneIndex < 0) || (OnAnimation()))
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

bool ModelSelectionPane::OnAnimation()
{
    return (m_animation.state() == QAbstractAnimation::Running);
}

void ModelSelectionPane::FlipAnimationStart(FlipDirection direction)
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

void ModelSelectionPane::ResetPostion(bool hasAnimation)
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

void ModelSelectionPane::on_refreshbtn_clicked()
{

}

void ModelSelectionPane::OnAnimationFinshed()
{
    PaneIndexDraw();
}

#if 1
bool ModelSelectionPane::event(QEvent *event)
{
#if 0
    if(event->type() == QEvent::Gesture) {
        LogManagerInst << LogManager::LogDegreeType_Normal << "QEvent::Gesture";
        QGestureEvent* gestureEvent = static_cast<QGestureEvent*>(event);
        QGesture *gesture = gestureEvent->gesture(Qt::PanGesture);
        if(NULL == gestureEvent) {
            LogManagerInst << LogManager::LogDegreeType_Error << "QEvent::Gesture is NULL";
        }else {
            QList<QGesture *> gestures = gestureEvent->gestures();
            QList<QGesture *> activeGestures = gestureEvent->activeGestures();

            LogManagerInst << "Gesture Value" << QString::number((int)gesture, 16);

            LogManagerInst << LogManager::LogDegreeType_Normal << "Gesture Count" << QString::number(gestures.count()) << "activeGestures Count" << QString::number(activeGestures.count());
            LogManagerInst << LogManager::LogDegreeType_Normal
                << (gestureEvent->gesture(Qt::TapGesture) != NULL?"TapGesture":"")
                << (gestureEvent->gesture(Qt::TapAndHoldGesture) != NULL?"TapAndHoldGesture":"")
                << ((NULL != gesture)?"PanGesture":"")
                << (gestureEvent->gesture(Qt::PinchGesture) != NULL?"PinchGesture":"")
                << (gestureEvent->gesture(Qt::SwipeGesture) != NULL?"SwipeGesture":"") ;
        }

        LogManagerInst << (gesture == NULL?"PanGesture == NULL":"PanGesture != NULL");
        if(gesture != NULL) {
            LogManagerInst << LogManager::LogDegreeType_Normal << "Handle Pan Gesture";
            QPanGesture* panGesture = static_cast<QPanGesture*>(gesture);
            LogManagerInst << LogManager::LogDegreeType_Normal << "Acceleration" << QString::number(panGesture->acceleration());
            LogManagerInst << LogManager::LogDegreeType_Normal << "delta" << QString::number(panGesture->delta().x());
            LogManagerInst << LogManager::LogDegreeType_Normal << "State" << QString::number(gesture->state());
            if(gesture->state() == Qt::GestureFinished) {
                if((panGesture->acceleration() > 2) || (panGesture->delta().x() > width()/2)) {
                    FlipAnimationStart(panGesture->delta().x() > 0 ? FlipDirection_Right : FlipDirection_Left);
                }
            } else {
                LogManagerInst << LogManager::LogDegreeType_Normal << "Position" << QString::number(ui->itemsetpane->x());
                QPoint targetPos = ui->itemsetpane->pos()+QPoint(panGesture->delta().x(),0);
                if(targetPos.x() > width()/4) {
                    targetPos.setX(width()/4);
                } else if(targetPos.x() < width() - m_itemSetPaneSize.width() - width()/4) {
                    targetPos.setX(width() - m_itemSetPaneSize.width() - width()/4);
                } else {

                }
                ui->itemsetpane->move(targetPos);
            }
        }
    }
#endif

#if 1
    /*****
     * 外面是阴天，我也成了阴天
     * 狗屎的Gesture
     * Document明明图示QPanGesture是一个手指
     * 源码里却他妈的是3个
     * 你他妈的逗我么
     * 我就是个秀逗
     * 明天dota只能用秀逗了
     * 悲剧啊悲剧啊
     * 我的时间
     * 如同我的身体老去
     *****/

    bool flipAction = false;
    static float last_x = 0.0f;
    static float touchBeginLastX = 0.0f;

    int deltaX = 0;
    float acceleration = 0.0f;

    switch(event->type()) {
#if 1
    case QEvent::TouchBegin: {
        LogManagerInst << LogManager::LogDegreeType_Normal  << "TouchBegin";
        flipAction = true;
        m_time.start();
        const QTouchEvent *touchEvent = static_cast<const QTouchEvent *>(event);
        QTouchEvent::TouchPoint touchPoint = touchEvent->touchPoints().first();
        last_x = touchPoint.pos().x();
        touchBeginLastX = touchPoint.pos().x();
    }
        break;
    case QEvent::TouchUpdate: {
        LogManagerInst << LogManager::LogDegreeType_Normal  << "TouchUpdate";
        flipAction = true;
        const QTouchEvent *touchEvent = static_cast<const QTouchEvent *>(event);
        QTouchEvent::TouchPoint touchPoint = touchEvent->touchPoints().first();
        deltaX = touchPoint.pos().x() - last_x;
        last_x = touchPoint.pos().x();
    }
        break;

    case QEvent::TouchEnd: {
        LogManagerInst << LogManager::LogDegreeType_Normal  << "TouchEnd";
        flipAction = true;
        const QTouchEvent *touchEvent = static_cast<const QTouchEvent *>(event);
        QTouchEvent::TouchPoint touchPoint = touchEvent->touchPoints().first();
        int elapseTime = m_time.elapsed();
        LogManagerInst << LogManager::LogDegreeType_Normal  << "elapseTime" << QString::number(elapseTime);
        deltaX = touchPoint.pos().x() - last_x;
        acceleration = (touchPoint.pos().x() - touchBeginLastX) / elapseTime;
    }
        break;
#else

    case QEvent::MouseButtonPress: {
        LogManagerInst << LogManager::LogDegreeType_Normal  << "TouchBegin";
        flipAction = true;
        m_time.start();
        const QMouseEvent *mouseEvent = static_cast<const QMouseEvent *>(event);
        last_x = mouseEvent->x();
        touchBeginLastX = mouseEvent->x();
    }
        break;

    case QEvent::MouseMove: {
        LogManagerInst << LogManager::LogDegreeType_Normal  << "TouchUpdate";
        flipAction = true;
        const QMouseEvent *mouseEvent = static_cast<const QMouseEvent *>(event);
        deltaX = mouseEvent->x() - last_x;
        last_x = mouseEvent->x();
    }
        break;

    case QEvent::MouseButtonRelease: {
        LogManagerInst << LogManager::LogDegreeType_Normal  << "TouchEnd";
        flipAction = true;
        int elapseTime = m_time.elapsed();
        LogManagerInst << LogManager::LogDegreeType_Normal  << "elapseTime" << QString::number(elapseTime);
        const QMouseEvent *mouseEvent = static_cast<const QMouseEvent *>(event);
        deltaX = mouseEvent->x() - last_x;
        acceleration = (mouseEvent->x() - touchBeginLastX) / elapseTime;
    }
        break;
#endif
    default:
        break;
    }

    if(flipAction) {
//        const QTouchEvent *touchEvent = static_cast<const QTouchEvent *>(event);
//        QTouchEvent::TouchPoint touchPoint = touchEvent->touchPoints().first();
        LogManagerInst << LogManager::LogDegreeType_Normal << "FlipInfo"
                       << "deltaX" << QString::number(deltaX)
                       << "acceleration" << QString::number(acceleration);
  //                     << "TouchPostion" << QString::number(touchPoint.pos().x())
  //                     << "TouchPoints" << QString::number(touchEvent->touchPoints().size());

        if(acceleration != 0) {
            if((acceleration > 1) || (deltaX > width()/2)) {
                FlipAnimationStart(deltaX > 0 ? FlipDirection_Right : FlipDirection_Left);
            }
        } else {
            LogManagerInst << LogManager::LogDegreeType_Normal << "Position" << QString::number(ui->itemsetpane->x());
            QPoint targetPos = ui->itemsetpane->pos()+QPoint(deltaX,0);
            if(targetPos.x() > width()/4) {
                targetPos.setX(width()/4);
            } else if(targetPos.x() < width() - m_itemSetPaneSize.width() - width()/4) {
                targetPos.setX(width() - m_itemSetPaneSize.width() - width()/4);
            } else {

            }

            ui->itemsetpane->move(targetPos);
            ui->itemsetpane->update();
        }

    }
#endif

    return PaneBase::event(event);
}
#endif

#if 0
bool ModelSelectionPane::eventFilter(QObject *obj, QEvent *event)
{
    bool flipAction = false;
    static float last_x = 0.0f;
    static float touchBeginLastX = 0.0f;

    int deltaX = 0;
    float acceleration = 0.0f;

    switch(event->type()) {
#if 0
    case QEvent::TouchBegin: {
        LogManagerInst << LogManager::LogDegreeType_Normal  << "TouchBegin";
        flipAction = true;
        m_time.start();
        const QTouchEvent *touchEvent = static_cast<const QTouchEvent *>(event);
        QTouchEvent::TouchPoint touchPoint = touchEvent->touchPoints().first();
        last_x = touchPoint.pos().x();
        touchBeginLastX = touchPoint.pos().x();
    }
        break;
    case QEvent::TouchUpdate: {
        LogManagerInst << LogManager::LogDegreeType_Normal  << "TouchUpdate";
        flipAction = true;
        const QTouchEvent *touchEvent = static_cast<const QTouchEvent *>(event);
        QTouchEvent::TouchPoint touchPoint = touchEvent->touchPoints().first();
        deltaX = touchPoint.pos().x() - last_x;
        last_x = touchPoint.pos().x();
    }
        break;

    case QEvent::TouchEnd: {
        LogManagerInst << LogManager::LogDegreeType_Normal  << "TouchEnd";
        flipAction = true;
        const QTouchEvent *touchEvent = static_cast<const QTouchEvent *>(event);
        QTouchEvent::TouchPoint touchPoint = touchEvent->touchPoints().first();
        int elapseTime = m_time.elapsed();
        LogManagerInst << LogManager::LogDegreeType_Normal  << "elapseTime" << QString::number(elapseTime);
        deltaX = touchPoint.pos().x() - last_x;
        acceleration = (touchPoint.pos().x() - touchBeginLastX) / elapseTime;
    }
        break;
#else

    case QEvent::MouseButtonPress: {
 //       LogManagerInst << LogManager::LogDegreeType_Normal  << "TouchBegin";
        flipAction = true;
        m_time.start();
        const QMouseEvent *mouseEvent = static_cast<const QMouseEvent *>(event);
        last_x = mouseEvent->x();
        touchBeginLastX = mouseEvent->x();
    }
        break;

    case QEvent::MouseMove: {
 //       LogManagerInst << LogManager::LogDegreeType_Normal  << "TouchUpdate";
        flipAction = true;
        const QMouseEvent *mouseEvent = static_cast<const QMouseEvent *>(event);
        deltaX = mouseEvent->x() - last_x;
        last_x = mouseEvent->x();
    }
        break;

    case QEvent::MouseButtonRelease: {
//        LogManagerInst << LogManager::LogDegreeType_Normal  << "TouchEnd";
        flipAction = true;
        int elapseTime = m_time.elapsed();
 //       LogManagerInst << LogManager::LogDegreeType_Normal  << "elapseTime" << QString::number(elapseTime);
        const QMouseEvent *mouseEvent = static_cast<const QMouseEvent *>(event);
        deltaX = mouseEvent->x() - last_x;
        acceleration = (mouseEvent->x() - touchBeginLastX) / elapseTime;
    }
        break;
#endif
    default:
        break;
    }

    if(flipAction) {
//        const QTouchEvent *touchEvent = static_cast<const QTouchEvent *>(event);
//        QTouchEvent::TouchPoint touchPoint = touchEvent->touchPoints().first();
//        LogManagerInst << LogManager::LogDegreeType_Normal << "FlipInfo"
 //                      << "deltaX" << QString::number(deltaX)
 //                      << "acceleration" << QString::number(acceleration);
  //                     << "TouchPostion" << QString::number(touchPoint.pos().x())
  //                     << "TouchPoints" << QString::number(touchEvent->touchPoints().size());

        if(acceleration != 0) {
            if((acceleration > 1) || (deltaX > width()/2)) {
                FlipAnimationStart(deltaX > 0 ? FlipDirection_Right : FlipDirection_Left);
            }
        } else {
   //         LogManagerInst << LogManager::LogDegreeType_Normal << "Position" << QString::number(ui->itemsetpane->x());
            QPoint targetPos = ui->itemsetpane->pos()+QPoint(deltaX,0);
            if(targetPos.x() > width()/4) {
                targetPos.setX(width()/4);
            } else if(targetPos.x() < width() - m_itemSetPaneSize.width() - width()/4) {
                targetPos.setX(width() - m_itemSetPaneSize.width() - width()/4);
            } else {

            }
            qDebug() << ui->itemsetpane->pos();
            ui->itemsetpane->move(targetPos);
    //        ui->itemsetpane->update();
    //        this->move(targetPos);
        }

    }

    return obj->event(event);
}
#endif
