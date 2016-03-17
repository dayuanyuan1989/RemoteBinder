#include "listwidget.h"
#include "ui_listwidget.h"

#include <QEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include <Qdebug>

ListWidget::ListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ListWidget),
    m_ItemSpace(4),
    m_selectedIndex(-1),
    m_cursorEnable(true),
    m_flip(false),
    m_runed(false),
    m_flipStatus(FlipStatus_NULL)
{
    ui->setupUi(this);

    init();
}

ListWidget::~ListWidget()
{
    final();

    delete ui;
}

void ListWidget::init()
{
    m_pWidgetListPane = new QWidget(ui->background);
    ui->curosr->setParent(m_pWidgetListPane);

    connect(&m_animManager, SIGNAL(valueHasChanged(QVariant)), this, SLOT(AnimationUpdate(QVariant)) );
    connect(&m_animManager, SIGNAL(finished()), this, SLOT(AnimationFinished()));

    ui->roll->hide();
    ui->curosr->hide();
}

void ListWidget::final()
{
   delete m_pWidgetListPane;
   m_pWidgetListPane = NULL;

   disconnect(&m_animManager, SIGNAL(valueHasChanged(QVariant)), this, SLOT(AnimationUpdate(QVariant)) );
   disconnect(&m_animManager, SIGNAL(finished()), this, SLOT(AnimationFinished()));
}

void ListWidget::mousePressEvent(QMouseEvent* e)
{
    e->accept();
    m_flipGrabBeginY = e->globalY();
}

void ListWidget::mouseMoveEvent(QMouseEvent *e)
{
    e->accept();

    const int pageBeginY = ui->background->y();
    const int pageEndY = -m_pWidgetListPane->height() + ui->background->height();
    static int lastY = 0;
    static int pressedPaneY = 0;
    int pandWidgetY = 0;
    if(m_flip == false){
        lastY = e->y();
        pressedPaneY = m_pWidgetListPane->y();
        m_flipTime.start();
 //       m_flipGrabBeginY = e->globalY();
    } else {
        bool moveOverFlag = RollChecked();
        if(moveOverFlag)
        {
            m_runed = true;
            int deltaY = e->y() - lastY;
            pandWidgetY = (pressedPaneY + deltaY);
            if(e->y() > lastY) {
                //down
                m_flipStatus = FlipStatus_Down;
                if(pressedPaneY + deltaY > pageBeginY) {
                    pandWidgetY = pageBeginY + ((pressedPaneY + deltaY) - pageBeginY)/2;
                }
            } else {
                //up
                m_flipStatus = FlipStatus_Up;
                if(pressedPaneY + deltaY < pageEndY) {
                    pandWidgetY = pageEndY + ((pressedPaneY + deltaY) - pageEndY)/2;
                }
            }
//            m_pWidgetListPane->move(QPoint(0, pressedPaneY + deltaY));
            m_pWidgetListPane->move(QPoint(0, pandWidgetY));
            AdjustRoll();
        }
    }
    m_flip = true;
}

void ListWidget::mouseReleaseEvent(QMouseEvent *e)
{
    m_runed = false;
    m_flip = false;

    float elapsedTime = m_flipTime.elapsed();
    int distance = e->globalY() - m_flipGrabBeginY;

    if(elapsedTime < 0.01) elapsedTime = 1;

    // not exist flip
    if(!RollChecked()) {
        if(m_cursorEnable) AdjustCursor();
        return;
    }

    float speed =  (float)distance / elapsedTime;
    const float uSpeed = qAbs(speed);
    QEasingCurve::Type type = QEasingCurve::Linear;
    const int pageBeginY = ui->background->y();
    const int pageEndY = -m_pWidgetListPane->height() + ui->background->height();
    int startPosY = m_pWidgetListPane->y();
    int targetPosY = 0;
    uint intervalTime = 0;

    qDebug() << "USpeed : " << uSpeed;

    // if runed, release animation, to start new animation
    if(m_animManager.OnRunning()) m_animManager.ReleaseAnimation();

    if(uSpeed < 0.7) {
        m_runed = false;
    } else if(uSpeed < 1.2) {
        m_runed = true;
        intervalTime = 1600 * uSpeed / 0.8;
        targetPosY = startPosY + speed * intervalTime / 5;
    } else{
        m_runed = true;
        intervalTime = 1600 * uSpeed / 0.8;
        targetPosY = startPosY + speed * intervalTime / 2;
    }

    if (targetPosY > pageBeginY) {
        intervalTime = intervalTime * qAbs((float)(pageBeginY-startPosY) / (targetPosY-startPosY));
        targetPosY = pageBeginY;
        type = QEasingCurve::OutBack;
    } else if (targetPosY < pageEndY){
        intervalTime = intervalTime * qAbs((float)(pageEndY-startPosY) / (targetPosY-startPosY));
        targetPosY = pageEndY;
        type = QEasingCurve::OutBack;
    } else {
        type = QEasingCurve::OutCirc;
    }

    if( ((startPosY > pageBeginY) || (startPosY < pageEndY)) ) {
        m_runed = true;
        type = QEasingCurve::OutCirc;
        intervalTime = 200;
        targetPosY = ((startPosY > pageBeginY) ? pageBeginY : pageEndY);
    }

    AdjustRoll();
    if(!m_animManager.OnRunning() && m_cursorEnable) AdjustCursor();

    if(m_runed) {
        m_animManager.AddAnimation(m_pWidgetListPane, UIAnimationManager::ActionType_Pos, m_pWidgetListPane->pos(), QPoint(m_pWidgetListPane->x(), targetPosY));
        m_animManager.SetDuration(intervalTime);
        m_animManager.SetEasingCurve(type);
        m_animManager.StartAnimation();
    }

    repaint();
}

void ListWidget::wheelEvent(QWheelEvent *e)
{
    const int pageBeginY = ui->background->y();
    const int pageEndY = -m_pWidgetListPane->height() + ui->background->height();
    int delta = ( (e->angleDelta().y() > 0) ? 60 : -60 );
    int posY = m_pWidgetListPane->y() + delta;

    if(m_animManager.OnRunning()) m_animManager.ReleaseAnimation();
    if(!RollChecked()) return;

    if(posY > pageBeginY) posY = pageBeginY;
    if(posY < pageEndY) posY = pageEndY;

    m_pWidgetListPane->move(0, posY);
    AdjustRoll();
}

void ListWidget::resizeEvent(QResizeEvent *e)
{
    QSize newSize = e->size();
    resize(newSize);
    ui->background->resize(newSize);
    AdjustGeometry();
    AdjustCursor();
    AdjustRoll();
}

bool ListWidget::eventFilter(QObject *obj, QEvent *e)
{
    int widgetSelectedIndex = -1;
    int  i = 0;
    for(i = 0; i < m_list.count(); i++) {
        if(m_list[i] == obj) {
            widgetSelectedIndex = i;
            break;
        }
    }
    if(widgetSelectedIndex != -1) {
        if(e->type() == QEvent::MouseButtonRelease) {
            if(m_selectedIndex != widgetSelectedIndex && !m_runed) {
                m_selectedIndex = widgetSelectedIndex;
                emit SelecteChanged(m_selectedIndex);
            }
            return ListWidget::event(e);
        }
    }

    return false;
}

void ListWidget::SetBackGroundColor(const QColor& color)
{
    QString style = QString("background-color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue());
    ui->background->setStyleSheet(style);
}

void ListWidget::AddWidgetItems(const QList<WidgetItem*>& list)
{
    m_pWidgetListPane->move(0, 0);
    m_list.append(list);
    SetParent();
    RemoveItemsFilter();
    InstallItemsFilter();
    AdjustGeometry();
    AdjustZOrder();
    update();
}

void ListWidget::AddWidgetItem(WidgetItem* pItem)
{
    m_pWidgetListPane->move(0, 0);
    m_list.append(pItem);
    SetParent();
    RemoveItemsFilter();
    InstallItemsFilter();
    AdjustGeometry();
    AdjustZOrder();
    update();
}

void ListWidget::Clear()
{
    RemoveItemsFilter();
    int i = 0;
    for(i = 0; i < m_list.count(); i++) {
        delete m_list[i];
        m_list[i] = NULL;
    }

    m_list.clear();
    ui->roll->hide();
    ui->curosr->hide();
    m_selectedIndex = -1;
    m_flipStatus = FlipStatus_NULL;
    update();
}

void ListWidget::InstallItemsFilter()
{
    foreach (WidgetItem* pItem, m_list) {
        pItem->installEventFilter(this);
    }
}

void ListWidget::RemoveItemsFilter()
{
    foreach (WidgetItem* pItem, m_list) {
        pItem->removeEventFilter(this);
    }
}

void ListWidget::SetParent()
{
    foreach (WidgetItem* pItem, m_list) {
        pItem->setParent(m_pWidgetListPane);
        pItem->show();
    }
}

void ListWidget::AdjustGeometry()
{
    QRect rect;
    int i = 0;
    for(i = 0; i < m_list.count(); i++) {
        rect.setTopLeft(QPoint( m_ItemSpace, (m_ItemSpace+i*(58+m_ItemSpace))) );
        rect.setBottomRight(QPoint( (width()-m_ItemSpace-1), (58+m_ItemSpace-1+i*(58+m_ItemSpace) )));
        m_list[i]->setGeometry(rect);
    }
    // reset m_widgetListPane size
    m_pWidgetListPane->setGeometry( QRect( 0, m_pWidgetListPane->y(), width(), (m_list.count() * (m_ItemSpace+58) + m_ItemSpace) ) );
}

void ListWidget::AdjustZOrder()
{
    ui->curosr->raise();
    ui->roll->raise();
}

void ListWidget::AdjustCursor()
{
    int index = m_selectedIndex;
    if( (index >= 0) && (index < m_list.count()) ) {
        ui->curosr->setGeometry(m_ItemSpace, (m_ItemSpace+index*(58+m_ItemSpace)), width()-m_ItemSpace*2, 58 );
        ui->curosr->show();        
    }
}

void ListWidget::SetCursorIndex(const int& index, bool visible)
{
    if( (index >= 0) && (index < m_list.count()) ) {
        m_selectedIndex = index;
        ui->curosr->setVisible(visible);
        ui->curosr->setGeometry(m_ItemSpace, (m_ItemSpace+index*(58+m_ItemSpace)), width()-m_ItemSpace*2, 58 );
    }
}

void ListWidget::AdjustRoll()
{
    int rollY;

    const int rollBeginY = ui->background->y();
    const int rollEndY = ui->background->geometry().bottom() - ui->roll->height() + 1;
    const int pageBeginY = ui->background->y();
    const int pageEndY = -m_pWidgetListPane->height() + ui->background->height();

    float factor = height()/(float)m_pWidgetListPane->height();

    if(RollChecked()) {
        if(!m_runed) {
            ui->roll->hide();
        } else {
            ui->roll->show();
        }

        if(m_pWidgetListPane->y() > pageBeginY) {
            // roll style
            SetRollHeight( (uint)(height()*factor) - qAbs((m_pWidgetListPane->y() - pageBeginY)*factor) );
            rollY = rollBeginY;
        } else if(m_pWidgetListPane->y() < pageEndY) {
            // roll style
            SetRollHeight( (uint)(height()*factor) - qAbs((m_pWidgetListPane->y() - pageEndY)*factor ) );
            rollY = rollEndY;
        } else {
            // roll style
            SetRollHeight( (uint)((height()*height())/(float)m_pWidgetListPane->height()) );
            rollY = (rollEndY - rollBeginY) * m_pWidgetListPane->y() / (pageEndY - pageBeginY);
        }
        ui->roll->move(width()-8, rollY);
    }
}

bool ListWidget::RollChecked()
{
    return (m_pWidgetListPane->height() > ui->background->height());
}

void ListWidget::AnimationUpdate(const QVariant& variant)
{
    if(variant.type() != QVariant::Point) return;
    AdjustRoll();
}

void ListWidget::AnimationFinished()
{
    m_runed = false;
    AdjustRoll();
}

void ListWidget::OnSelectChanged(int)
{

}

void ListWidget::SetRollHeight(uint height)
{
    ui->roll->resize(ui->roll->width(), (ui->roll->width()>(int)height)?ui->roll->width():height);
}

void ListWidget::SetRollColor(const QColor& color)
{
    QString style = QString("background-color: rgb(%1, %2, %3);").arg(color.red()).arg(color.green()).arg(color.blue());
    ui->roll->setStyleSheet(style);
}
