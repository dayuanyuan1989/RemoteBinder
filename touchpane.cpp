#include "touchpane.h"
#include "ui_touchpane.h"
#include "resourcemanager.h"
#include "logmanager.h"

#include <QtMath>
#include <QResizeEvent>
#include <QEvent>
#include <QTouchEvent>

#include <QDebug>

TouchPane::TouchPane(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TouchPane),
    m_sensitivity(1.0f),
    m_touchhandle(false)
{
    ui->setupUi(this);
    // set to accept touch event
    setAttribute(Qt::WA_AcceptTouchEvents);
    ui->background->installEventFilter(this);
    ui->background->setAttribute(Qt::WA_AcceptTouchEvents);
    ui->grphelpbox->hide();
    ui->label->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->label_2->setAttribute(Qt::WA_TransparentForMouseEvents);
    init();
}

TouchPane::~TouchPane()
{
    final();
    ui->background->removeEventFilter(this);
    delete ui;
}

void TouchPane::resizeEvent(QResizeEvent *e)
{
    resize(e->size());
    ui->background->resize(e->size());
    ComponentsGeometryAdjust();
}

void TouchPane::init()
{
    InstallEventFilter();
    ComponentsGeometryAdjust();
}

void TouchPane::final()
{
    RemoveEventFilter();
}

void TouchPane::InstallEventFilter()
{
    ui->background->installEventFilter(this);
}

void TouchPane::RemoveEventFilter()
{
    ui->background->removeEventFilter(this);
}

bool TouchPane::event(QEvent *e)
{
 //   HandleTouchEvent(e);

    return QWidget::event(e);
}

void TouchPane::HandleMouseEvent(QEvent *e)
{
    static bool firstFrame = true;
    const QRect touchPaneRect(0, 0, width(), height());
    switch (e->type()) {
    case QEvent::MouseButtonPress:
//        qDebug() << "MouseButtonPress";
        firstFrame = true;
        break;
    case QEvent::MouseMove:
    {
        QMouseEvent* mouse_e = reinterpret_cast<QMouseEvent*>(e);

        if(  !touchPaneRect.contains(mouse_e->pos()) )    return;

        Qt::MouseButton btn;
        if(mouse_e->buttons() & Qt::LeftButton) {
            btn = Qt::LeftButton;
        } else {
            btn = Qt::RightButton;
        }

        QPoint mousePos = mouse_e->pos();
        QSize paneSize = size();
        QString mouseEventData = GetMouseSenderMsg(btn, mousePos, paneSize, !firstFrame);
        if(firstFrame) firstFrame = false;
        emit MousePaneUpdate(mouseEventData);
    }

        break;
    case QEvent::MouseButtonRelease:
 //       qDebug() << "MouseButtonRelease";
        firstFrame = true;
        break;
    default:
        break;
    }
}

void TouchPane::HandleTouchEvent(QEvent *e)
{
    static bool firstFrame = true;
    static double len;
    static double factor;
    static bool secondFrame = false;
    const static double TagValue = 1e-15;

    switch(e->type()) {
    case QEvent::TouchBegin:
 //       LogManagerInst << LogManager::LogDegreeType_Normal << "TouchBegin";
//        LogManagerInst << LogManager::LogDegreeType_Normal << "TouchCount" << QString::number(static_cast<const QTouchEvent *>(e)->touchPoints().count());
        firstFrame = true;
        secondFrame = false;
        break;
    case QEvent::TouchUpdate:
    {
//        LogManagerInst << LogManager::LogDegreeType_Normal << "TouchUpdate";
        const QTouchEvent *touch_e = static_cast<const QTouchEvent *>(e);
        int touchCount = touch_e->touchPoints().count();
//        LogManagerInst << LogManager::LogDegreeType_Normal << "TouchCount" << QString::number(touchCount);

        if( (1 == touchCount) ) {
            m_touchhandle = false;
            firstFrame = true;
        }

        // touch event not to handle rotate
        if((touchCount != 2))
            return;

        m_touchhandle = true;

        QPointF touchPoint0 = touch_e->touchPoints().at(0).pos();
        QPointF touchPoint1 = touch_e->touchPoints().at(1).pos();

        QPointF deltaPoint = touchPoint0 - touchPoint1;

        if(firstFrame) {
            firstFrame = false;
            len = sqrt( pow(deltaPoint.x(), 2) + pow(deltaPoint.y(), 2) );
        } else {
            double deltaLen = sqrt( pow(deltaPoint.x(), 2) + pow(deltaPoint.y(), 2) ) - len;
            len = sqrt( pow(deltaPoint.x(), 2) + pow(deltaPoint.y(), 2) );
            double maxLen = sqrt( pow(width(), 2) + pow(height(), 2) );
            double curFactor = deltaLen / maxLen;
            if(secondFrame && ( (curFactor / factor < -100) || qAbs(curFactor) <  TagValue) ) {
                m_touchhandle = false;
                firstFrame = true;
                secondFrame = false;
                return;
            }

            secondFrame = true;

            emit MousePaneUpdate( QString("TouchEvent:Scale:%1;").arg(curFactor) );
            factor = curFactor;
        }
#if 0
        mousePos = QPoint((int)mousePosF.x(), (int)mousePosF.y());
        QSize paneSize = size();
        QString mouseEventData = GetMouseSenderMsg(btn, mousePos, paneSize, !firstFrame);
        if(firstFrame) firstFrame = false;
        emit MousePaneUpdate(mouseEventData);
#endif
    }
        break;

    case QEvent::TouchEnd:
        m_touchhandle = false;
        secondFrame = false;
        LogManagerInst << LogManager::LogDegreeType_Normal << "TouchEnd";
        firstFrame = true;
        break;
    default:

        break;
    }
}
#if 1
bool TouchPane::eventFilter(QObject *obj, QEvent *e)
{
    e->accept();
    if(obj == ui->background) {
  //      LogManagerInst << "EventFilter";
        HandleTouchEvent(e);

        if(!m_touchhandle) {
            HandleMouseEvent(e);
        }
    }

    return obj->event(e);
}
#endif

void TouchPane::SetBackgroundTransparent(bool checked)
{
    static QString str = ui->background->styleSheet();
    if(checked)
        ui->background->setStyleSheet(
            "#background{   \
                background-color: rgba(221, 221, 222, 02);    \
            }"
        );
    else
        ui->background->setStyleSheet(
            "#background{   \
                background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(174, 174, 174, 255), stop:1 rgba(221, 221, 222, 255));    \
            }"
        );
}

QString TouchPane::GetMouseSenderMsg(Qt::MouseButton btnIndex, const QPoint& pos, const QSize& size, bool mouseMoving)
{
    int posValue = 0;
    posValue = pos.x() << 16;
    posValue += pos.y();
    QString posStr = QString::number(posValue);

    int sizeValue = 0;
    sizeValue = size.width() << 16;
    sizeValue += size.height();
    QString sizeStr = QString::number(sizeValue);

    QString mouseData = QString("MouseEvent:%1:%2:Size:%3:Sensitivity:%4:%5;").arg(btnIndex==Qt::LeftButton?QString("Rotate"):QString("Scale"))
            .arg(mouseMoving?QString("Moving"):QString("Start")).arg(sizeStr).arg(QString::number(m_sensitivity)).arg(posStr);

    return mouseData;
}

void TouchPane::ComponentsGeometryAdjust()
{
    const int w = width();
    const int h = height();

    ui->grphelpbox->move(w-28-ui->grphelpbox->width(), h-10-ui->grphelpbox->height());
}

void TouchPane::HelpPaneVisible(bool visible)
{
    ui->grphelpbox->setVisible(visible);
}
